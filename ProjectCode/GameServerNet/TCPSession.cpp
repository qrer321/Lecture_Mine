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

void TCPSession::OnCallBack(PtrWTCPSession weakSession, BOOL result, DWORD numberOfBytes, LPOVERLAPPED lpOverlapped)
{
	if (nullptr == lpOverlapped)
		return;

	auto overlappedPtr = reinterpret_cast<GameServerOverlapped*>(reinterpret_cast<char*>(lpOverlapped) - sizeof(void*));

	// recv던 send던 다형성을 통해 적절한 클래스의 Execute 함수 호출
	overlappedPtr->Execute(result, numberOfBytes);
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

bool TCPSession::BindQueue(const GameServerQueue& taskQueue)
{
	PtrWTCPSession sessionPtr = std::dynamic_pointer_cast<TCPSession>(shared_from_this());
	return taskQueue.NetworkBind(m_SessionSocket,
	                             [sessionPtr](auto&& result, auto&& numberOfBytes, auto&& lpOverlapped)
	                             {
		                             return TCPSession::OnCallBack(sessionPtr, std::forward<decltype(result)>(result),
		                                                           std::forward<decltype(numberOfBytes)>(numberOfBytes),
		                                                           std::forward<decltype(lpOverlapped)>(lpOverlapped));
	                             });
}

void TCPSession::AcceptBindOn()
{
	m_IsAcceptBind = true;
}

// Session이 Send를 완료했을 경우 m_SendOverlappedPool에 Overlapped를 반납한다.
void TCPSession::OnSendComplete(SendOverlapped* sendOverlapped)
{
	m_SendOverlappedPool.Push(sendOverlapped);
}

bool TCPSession::Send(const std::vector<unsigned char>& buffer)
{
	if (buffer.empty())
		return false;

	DWORD byteSize = 0;
	DWORD flag = 0;

	SendOverlapped* sendOverlapped = m_SendOverlappedPool.Pop();
	sendOverlapped->SetTCPSession(this);
	sendOverlapped->CopyFrom(buffer);

	// WSASend가 완료된 경우 설정된 Overlapped 주소 ==> GameServerQueue::WorkDefault() 함수에서 설정된 task에 의해
	// TCPSession::OnSendComplete 함수가 호출되어 사용한 SendOverlapped를 Pool에 반환한다.
	int error = WSASend(m_SessionSocket, sendOverlapped->GetBuffer(), 1, &byteSize, flag,
						sendOverlapped->GetOverlapped(), nullptr);

	if (SOCKET_ERROR == error)
	{
		if (WSA_IO_PENDING == WSAGetLastError())
		{
			GameServerDebug::GetLastErrorPrint();
			return false;
		}
	}

	return true;
}

void TCPSession::OnRecv(const char* data, DWORD byteSize)
{
	// 프로토콜 처리
	// byteSize만큼 vector resize
	std::vector<unsigned char> outputBuffer = std::vector<unsigned char>(byteSize);
	memcpy_s(&outputBuffer[0], byteSize, data, byteSize);

	if (nullptr != m_RecvCallBack)
	{
		m_RecvCallBack(std::dynamic_pointer_cast<TCPSession>(shared_from_this()), outputBuffer);
	}

	// 잘못된 혹은 수상한 패킷이 들어올 경우
	// if () 문을 통해 패킷을 검사하고
	// 정상적인 패킷을 보내지 않은 클라이언트라면
	// Close(force_close = true); 함수 호출로 서버에서 내보낸다

	if (false == m_CallClose)
	{
		RecvRequest();
	}
}

void TCPSession::RecvRequest()
{
	DWORD recvByte = 0, dwFlag = 0;

	// 새로운 리시브를 받을 때 마다 Overlapped를 Clear 해준다
	m_RecvOverlapped->Clear();

	if (SOCKET_ERROR == WSARecv(
		m_SessionSocket,
		m_RecvOverlapped->GetWSABuffer(),
		1,
		&recvByte,
		&dwFlag,
		m_RecvOverlapped->GetOverlapped(),
		nullptr
	))
	{
		int error = WSAGetLastError();
		if (WSA_IO_PENDING != error)
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
		GameServerDebug::LogError("ReceiveCallBack Is NULL");
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
	TCPListener* parentListener = GetParent<TCPListener>();

	if (nullptr == parentListener)
	{
		GameServerDebug::AssertDebugMsg("리스너와 연결되지 않은 세션이 존재합니다.");
		return;
	}

	parentListener->CloseSession(std::dynamic_pointer_cast<TCPSession>(shared_from_this()));

	m_CallClose = false;

	// 새로운 Connection ID를 부여받아야 한다.
	// UniqueId의 특성상 새로 들어오게 된 세션의 경우 새로운 UniqueId를 부여해주어야 하기 때문이다.
	m_ConnectId = GameServerUnique::GetNextUniqueId();
}

void TCPSession::Close(bool forceClose)
{
	if (nullptr != m_CloseCallBack)
	{
		m_CloseCallBack(std::dynamic_pointer_cast<TCPSession>(shared_from_this()));
	}

	m_CallClose = true;

	if (false == forceClose)
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
		GameServerDebug::AssertDebugMsg("유효하지 않은 소켓을 디스커넥트 하려고 했습니다");
		return;
	}

	if (nullptr == m_DisconnectOverlapped)
	{
		GameServerDebug::AssertDebugMsg("m_DisconnectOverlapped가 정상적으로 만들어지지 않았습니다");
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
		int error = WSAGetLastError();
		if (WSA_IO_PENDING != error)
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