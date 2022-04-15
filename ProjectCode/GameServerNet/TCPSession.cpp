#include "PreCompile.h"
#include "TCPSession.h"
#include "GameServerOverlapped.h"
#include "TCPListener.h"

TCPSession::TCPSession()
	: m_SessionSocket(NULL)
	, m_ConnectId(0)
	, m_IsAcceptBind(false)
	, m_RecvOverlapped(nullptr)
	, m_DisconnectOverlapped(nullptr)
	, m_CallClose(false)
{}

TCPSession::~TCPSession()
{
	delete m_RecvOverlapped;
	delete m_DisconnectOverlapped;
}

TCPSession::TCPSession(TCPSession&& other) noexcept
	: m_SessionSocket(other.m_SessionSocket)
	, m_ConnectId(other.m_ConnectId)
	, m_IsAcceptBind(other.m_IsAcceptBind)
	, m_RecvOverlapped(other.m_RecvOverlapped)
	, m_DisconnectOverlapped(other.m_DisconnectOverlapped)
	, m_CallClose(other.m_CallClose)
{
	other.m_RecvOverlapped = nullptr;
	other.m_DisconnectOverlapped = nullptr;
}

bool TCPSession::Initialize()
{
	// 접속자를 받으려는 소켓
	// 비동기 액셉트를 하려는 소켓이다
	m_SessionSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, NULL, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET == m_SessionSocket)
	{
		GameServerDebug::GetLastErrorPrint();
		return false;
	}

	int error = 0;
	// NoDelay 옵션
	{
		constexpr BOOL noDelayOption = TRUE;
		error = setsockopt(m_SessionSocket, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<const char*>(&noDelayOption), sizeof(noDelayOption));
		if (SOCKET_ERROR == error)
		{
			GameServerDebug::GetLastErrorPrint();
			Close();
			return false;
		}
	}

	// ReUse 옵션
	{
		constexpr BOOL reuseOption = TRUE;
		error = setsockopt(m_SessionSocket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&reuseOption), sizeof(reuseOption));
		if (SOCKET_ERROR == error)
		{
			GameServerDebug::GetLastErrorPrint();
			Close();
			return false;
		}
	}

	// 연결관련 옵션
	{
		LINGER linger;
		linger.l_onoff = 1;		// 보낼 데이터 바로 폐기
		linger.l_linger = 0;	// 연결 종료 과정은 거친다

		error = setsockopt(m_SessionSocket, SOL_SOCKET, SO_LINGER, reinterpret_cast<const char*>(&linger), sizeof(linger));
		if (SOCKET_ERROR == error)
		{
			GameServerDebug::GetLastErrorPrint();
			Close();
			return false;
		}
	}

	m_ConnectId = GameServerUnique::GetNextUniqueId();

	if (nullptr == m_RecvOverlapped)
	{
		m_RecvOverlapped = new RecvOverlapped(this);
	}

	if (nullptr == m_DisconnectOverlapped)
	{
		m_DisconnectOverlapped = new DisconnectOverlapped(this);
	}

	return true;
}

bool TCPSession::BindQueue(const GameServerQueue& task_queue)
{
	return task_queue.NetworkBind(m_SessionSocket,
		[&](auto&& result, auto&& number_of_bytes, auto&& overlapped)
		{
			OnCallBack(std::forward<decltype(result)>(result),
			           std::forward<decltype(number_of_bytes)>(number_of_bytes),
			           std::forward<decltype(overlapped)>(overlapped));
		});
}

void TCPSession::OnCallBack(BOOL result, DWORD number_of_bytes, LPOVERLAPPED overlapped)
{
	if (nullptr == overlapped)
		return;

	const auto server_overlapped = reinterpret_cast<GameServerOverlapped*>(reinterpret_cast<char*>(overlapped) - sizeof(void*));

	// recv던 send던 다형성을 통해 적절한 클래스의 Execute 함수 호출
	server_overlapped->Execute(result, number_of_bytes);
}

// Session이 Send를 완료했을 경우 m_SendOverlappedPool에 Overlapped를 반납한다.
void TCPSession::OnSendComplete(SendOverlapped* send_overlapped)
{
	m_SendOverlappedPool.Push(send_overlapped);
}

void TCPSession::OnRecv(const char* recv_buffer, DWORD number_of_bytes)
{
	if (nullptr == m_RecvCallBack)
	{
		return;
	}

	// 프로토콜 처리
	// byteSize만큼 vector resize
	std::vector<unsigned char> buffer = std::vector<unsigned char>(number_of_bytes);
	memcpy_s(&buffer[0], number_of_bytes, recv_buffer, number_of_bytes);

	m_RecvCallBack(DynamicCast<TCPSession>(), buffer);

	// 잘못된 혹은 수상한 패킷이 들어올 경우
	// if () 문을 통해 패킷을 검사하고
	// 정상적인 패킷을 보내지 않은 클라이언트라면
	// Close(force_close = true); 함수 호출로 서버에서 내보낸다
	if (false == m_CallClose)
	{
		Recv();
	}
}

bool TCPSession::Send(const std::vector<unsigned char>& send_data)
{
	if (send_data.empty())
	{
		return false;
	}

	SendOverlapped* send_overlapped = m_SendOverlappedPool.Pop();
	send_overlapped->SetTCPSession(this);
	send_overlapped->CopyFrom(send_data);

	DWORD sent_bytes = 0;

	// WSASend가 완료된 경우 설정된 Overlapped 주소 ==> GameServerQueue::WorkDefault() 함수에서 설정된 task에 의해
	// TCPSession::OnSendComplete 함수가 호출되어 사용한 SendOverlapped를 Pool에 반환한다.
	if (SOCKET_ERROR == WSASend(
		m_SessionSocket,
		send_overlapped->GetBuffer(),
		1,
		&sent_bytes,
		0,
		send_overlapped->GetOverlapped(),
		nullptr))
	{
		if (WSA_IO_PENDING == WSAGetLastError())
		{
			GameServerDebug::GetLastErrorPrint();
			return false;
		}
	}

	return true;
}

void TCPSession::Recv()
{
	if (INVALID_SOCKET == m_SessionSocket)
	{
		return;
	}

	DWORD recvd_bytes = 0, flags = 0;

	// 새로운 리시브를 받을 때 마다 Overlapped를 Clear 해준다
	m_RecvOverlapped->Clear();

	if (SOCKET_ERROR == WSARecv(
		m_SessionSocket,
		m_RecvOverlapped->GetWSABuffer(),
		1,
		&recvd_bytes,
		&flags,
		m_RecvOverlapped->GetOverlapped(),
		nullptr
	))
	{
		if (WSA_IO_PENDING != WSAGetLastError())
		{
			GameServerDebug::GetLastErrorPrint();
			return;
		}
	}
}

void TCPSession::SetCallBack(const std::function<void(PtrSTCPSession, const std::vector<unsigned char>&)>& recv_callback, const std::function<void(PtrSTCPSession)>& close_callback)
{
	if (nullptr == recv_callback)
	{
		GameServerDebug::LogError("Receive Callback Is Nullptr");
	}
	if (nullptr == close_callback)
	{
		GameServerDebug::LogError("Close Callback Is Nullptr");
	}

	m_RecvCallBack = recv_callback;
	m_CloseCallBack = close_callback;
}

void TCPSession::SetRecvCallBack(const std::function<void(PtrSTCPSession, const std::vector<unsigned char>&)>& callback)
{
	if (nullptr == callback)
	{
		GameServerDebug::LogError("Receive Callback Is Nullptr");
	}

	m_RecvCallBack = callback;
}

void TCPSession::SetCloseCallBack(const std::function<void(PtrSTCPSession)>& callback)
{
	if (nullptr == callback)
	{
		GameServerDebug::LogError("Close Callback Is Nullptr");
	}

	m_CloseCallBack = callback;
}

// Disconnect를 통한 등록해제
void TCPSession::UnRegisterSession()
{
	TCPListener* tcp_listener = GetParent<TCPListener>();
	if (nullptr == tcp_listener)
	{
		GameServerDebug::AssertDebugMsg("TCP Listener Is Nullptr");
		return;
	}

	tcp_listener->CloseSession(DynamicCast<TCPSession>());

	m_CallClose = false;

	// 새로운 Connection ID를 부여받아야 한다.
	// UniqueId의 특성상 새로 들어오게 된 세션의 경우 새로운 UniqueId를 부여해주어야 하기 때문이다.
	m_ConnectId = GameServerUnique::GetNextUniqueId();
}

void TCPSession::Close(bool force_close)
{
	if (nullptr != m_CloseCallBack)
	{
		m_CloseCallBack(DynamicCast<TCPSession>());
	}

	m_CallClose = true;

	if (false == force_close)
	{
		DisconnectSocket();
	}
	else
	{
		// 강제종료
		CloseSocket();
	}
}

void TCPSession::DisconnectSocket()
{
	if (INVALID_SOCKET == m_SessionSocket)
	{
		GameServerDebug::AssertDebugMsg("Invalid Session Socket Disconnected");
		return;
	}

	if (nullptr == m_DisconnectOverlapped)
	{
		GameServerDebug::AssertDebugMsg("Disconnect Overlapped Is Nullptr");
		return;
	}

	/*
	 * TransmitFile(hSocket, hFile, nNumberOfBytesToWrite, nNumberOfBytesPerSend, lpOverlapped, lpTransmitBuffers, dwReserved)
	 * TransmitFile의 기본적인 기능은 연결된 소켓 핸들을 통해 파일 데이터를 전송하는 것이다.
	 *
	 * 해당 코드에서는 TF_DISCONNECT와 TF_REUSE_SOCKET 플래그를 이용하여
	 * 클라이언트와 연결된 소켓을 Disconnect하고 Reuse 하겠다고 OS에게 알려주기 위해 사용한다.
	 * 클라이언트의 접속이 종료되면 OS는 아직 세션을 유지하고 있는데
	 * 해당 세션과의 연결끊기 작업이 완료되면 OS에게 DisconnectOverlapped로 보내달라고 요청하는 것 이다.
	 *
	 * TransmitFile이 반환될 경우 m_SessionSocket에 바인드되어있는
	 * OnCallBack 함수로 Overlapped 구조체와 함께 넘어온다
	 * 해당 Overlapped는 DisconnectOverlapped이기 때문에
	 * DisconnectOverlapped::Execute()가 호출될 것이다.
	 *
	 * @param
	 * SOCKET hSocket								: 연결 종료를 할 소켓, SessionSocket
	 *
	 * LPOVERLAPPED lpOverlapped					: 연결 종료 시 사용할 Overlapped 구조체 포인터
	 *												  Overlapped I/O일 경우 TransmitFile이 반환되기 전에 해당 작업이 완료되지 않을 수 있다.
	 *												  이 경우 ERROR_IO_PENDING 혹은 WSA_IO_PENDING 오류 값을 반환한다.
	 *												  Windows는 요청이 완료되면 OVERLAPPED 구조체의 hEvent 멤버 혹은 hSocket에 이벤트 신호를 보낸다
	 *
	 * DWORD dwReserved								: 동작을 설정하는데 사용되는 플래그 집합
	 *												  TF_DISCONNECT : 연결 끊기
	 *												  TF_REUSE_SOCKET : 재사용을 위한 플래그, TCP_TIME_WAIT에 종속되어 호출이 지연될 수 있다.
	 */
	BOOL result = TransmitFile(m_SessionSocket, NULL, 0, 0,
								m_DisconnectOverlapped->GetOverlapped(), nullptr, TF_DISCONNECT | TF_REUSE_SOCKET);

	if (FALSE == result)
	{
		if (WSA_IO_PENDING != WSAGetLastError())
		{
			GameServerDebug::GetLastErrorPrint();
		}
	}
}

void TCPSession::CloseSocket()
{
	if (INVALID_SOCKET == m_SessionSocket)
	{
		shutdown(m_SessionSocket, SD_BOTH);
		closesocket(m_SessionSocket);
		m_SessionSocket = INVALID_SOCKET;
	}
}