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

	// recv�� send�� �������� ���� ������ Ŭ������ Execute �Լ� ȣ��
	overlappedPtr->Execute(result, numberOfBytes);
}

bool TCPSession::Initialize()
{
	// �����ڸ� �������� ����
	// �񵿱� �׼�Ʈ�� �Ϸ��� �����̴�
	m_SessionSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, NULL, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET == m_SessionSocket)
	{
		GameServerDebug::GetLastErrorPrint();
		return false;
	}

	int error = 0;
	// NoDelay �ɼ�
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

	// ReUse �ɼ�
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

	// ������� �ɼ�
	{
		LINGER linger;
		linger.l_onoff = 1;		// ���� ������ �ٷ� ���
		linger.l_linger = 0;	// ���� ���� ������ ��ģ��

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

// Session�� Send�� �Ϸ����� ��� m_SendOverlappedPool�� Overlapped�� �ݳ��Ѵ�.
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

	// WSASend�� �Ϸ�� ��� ������ Overlapped �ּ� ==> GameServerQueue::WorkDefault() �Լ����� ������ task�� ����
	// TCPSession::OnSendComplete �Լ��� ȣ��Ǿ� ����� SendOverlapped�� Pool�� ��ȯ�Ѵ�.
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
	// �������� ó��
	// byteSize��ŭ vector resize
	std::vector<unsigned char> outputBuffer = std::vector<unsigned char>(byteSize);
	memcpy_s(&outputBuffer[0], byteSize, data, byteSize);

	if (nullptr != m_RecvCallBack)
	{
		m_RecvCallBack(std::dynamic_pointer_cast<TCPSession>(shared_from_this()), outputBuffer);
	}

	// �߸��� Ȥ�� ������ ��Ŷ�� ���� ���
	// if () ���� ���� ��Ŷ�� �˻��ϰ�
	// �������� ��Ŷ�� ������ ���� Ŭ���̾�Ʈ���
	// Close(force_close = true); �Լ� ȣ��� �������� ��������

	if (false == m_CallClose)
	{
		RecvRequest();
	}
}

void TCPSession::RecvRequest()
{
	DWORD recvByte = 0, dwFlag = 0;

	// ���ο� ���ú긦 ���� �� ���� Overlapped�� Clear ���ش�
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

// Disconnect�� ���� �������
void TCPSession::UnRegisterSession()
{
	TCPListener* parentListener = GetParent<TCPListener>();

	if (nullptr == parentListener)
	{
		GameServerDebug::AssertDebugMsg("�����ʿ� ������� ���� ������ �����մϴ�.");
		return;
	}

	parentListener->CloseSession(std::dynamic_pointer_cast<TCPSession>(shared_from_this()));

	m_CallClose = false;

	// ���ο� Connection ID�� �ο��޾ƾ� �Ѵ�.
	// UniqueId�� Ư���� ���� ������ �� ������ ��� ���ο� UniqueId�� �ο����־�� �ϱ� �����̴�.
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
		// ��������
		CloseSocket();
	}
}

void TCPSession::DisconnectSocket()
{
	if (INVALID_SOCKET == m_SessionSocket)
	{
		GameServerDebug::AssertDebugMsg("��ȿ���� ���� ������ ��Ŀ��Ʈ �Ϸ��� �߽��ϴ�");
		return;
	}

	if (nullptr == m_DisconnectOverlapped)
	{
		GameServerDebug::AssertDebugMsg("m_DisconnectOverlapped�� ���������� ��������� �ʾҽ��ϴ�");
		return;
	}

	/*
	 * TransmitFile(hSocket, hFile, nNumberOfBytesToWrite, nNumberOfBytesPerSend, lpOverlapped, lpTransmitBuffers, dwReserved)
	 * TransmitFile�� �⺻���� ����� ����� ���� �ڵ��� ���� ���� �����͸� �����ϴ� ���̴�.
	 *
	 * �ش� �ڵ忡���� TF_DISCONNECT�� TF_REUSE_SOCKET �÷��׸� �̿��Ͽ�
	 * Ŭ���̾�Ʈ�� ����� ������ Disconnect�ϰ� Reuse �ϰڴٰ� OS���� �˷��ֱ� ���� ����Ѵ�.
	 * Ŭ���̾�Ʈ�� ������ ����Ǹ� OS�� ���� ������ �����ϰ� �ִµ�
	 * �ش� ���ǰ��� ������� �۾��� �Ϸ�Ǹ� OS���� DisconnectOverlapped�� �����޶�� ��û�ϴ� �� �̴�.
	 *
	 * TransmitFile�� ��ȯ�� ��� m_SessionSocket�� ���ε�Ǿ��ִ�
	 * OnCallBack �Լ��� Overlapped ����ü�� �Բ� �Ѿ�´�
	 * �ش� Overlapped�� DisconnectOverlapped�̱� ������
	 * DisconnectOverlapped::Execute()�� ȣ��� ���̴�.
	 *
	 * @param
	 * SOCKET hSocket								: ���� ���Ḧ �� ����, SessionSocket
	 *
	 * LPOVERLAPPED lpOverlapped					: ���� ���� �� ����� Overlapped ����ü ������
	 *												  Overlapped I/O�� ��� TransmitFile�� ��ȯ�Ǳ� ���� �ش� �۾��� �Ϸ���� ���� �� �ִ�.
	 *												  �� ��� ERROR_IO_PENDING Ȥ�� WSA_IO_PENDING ���� ���� ��ȯ�Ѵ�.
	 *												  Windows�� ��û�� �Ϸ�Ǹ� OVERLAPPED ����ü�� hEvent ��� Ȥ�� hSocket�� �̺�Ʈ ��ȣ�� ������
	 *
	 * DWORD dwReserved								: ������ �����ϴµ� ���Ǵ� �÷��� ����
	 *												  TF_DISCONNECT : ���� ����
	 *												  TF_REUSE_SOCKET : ������ ���� �÷���, TCP_TIME_WAIT�� ���ӵǾ� ȣ���� ������ �� �ִ�.
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