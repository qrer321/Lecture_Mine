#include "PreCompile.h"
#include "TCPListener.h"
#include "TCPSession.h"
#include "ServerHelper.h"
#include "SocketAddress.h"
#include "IPEndPoint.h"
#include "GameServerOverlapped.h"

TCPListener::TCPListener()
	: m_ListenSocket(NULL)
	, m_TaskQueue(nullptr)
	, m_AcceptCallBack(nullptr)
{
}

TCPListener::~TCPListener()
{
}

TCPListener::TCPListener(TCPListener&& other) noexcept
	: m_ListenSocket(other.m_ListenSocket)
	, m_TaskQueue(other.m_TaskQueue)
	, m_AcceptCallBack(std::move(other.m_AcceptCallBack))
{
	other.m_ListenSocket = INVALID_SOCKET;
	other.m_TaskQueue = nullptr;
}

bool TCPListener::Initialize(const IPEndPoint& end_point, const std::function<void(std::shared_ptr<TCPSession>)>& accept_callback)
{
	// WSAStartUp
	ServerHelper::ServerStartup();

	// �ּҿ� ��� ���� ��Ŷ���� ������ ���� ����Ʈ ������ ����ȭ�Ѵ�.
	SocketAddress address = end_point.Serialize();

	// �񵿱� �׼�Ʈ�� �Ϸ��� ���� ����
	m_ListenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, NULL, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET == m_ListenSocket)
	{
		GameServerDebug::GetLastErrorPrint();
		return false;
	}
	
	int error = bind(m_ListenSocket, reinterpret_cast<const sockaddr*>(address.GetBuffer()), static_cast<int>(address.GetBufferLength()));
	if (SOCKET_ERROR == error)
	{
		GameServerDebug::GetLastErrorPrint();
		Close();
		return false;
	}

	// m_ListenSocket �ɼ� ����
	BOOL on = TRUE;
	error = setsockopt(m_ListenSocket, SOL_SOCKET, SO_CONDITIONAL_ACCEPT, reinterpret_cast<const char*>(&on), sizeof(on));
	if (SOCKET_ERROR == error)
	{
		GameServerDebug::GetLastErrorPrint();
		Close();
		return false;
	}

	// ��α� ���� ����
	error = listen(m_ListenSocket, SOMAXCONN);
	if (SOCKET_ERROR == error)
	{
		GameServerDebug::GetLastErrorPrint();
		Close();
		return false;
	}

	m_ListenEndPoint = end_point;
	m_AcceptCallBack = accept_callback;

	return true;
}

bool TCPListener::BindQueue(const GameServerQueue& task_queue)
{
	m_TaskQueue = &task_queue;

	// ���ܳ��� �Լ��� ����
	// NetworkBind�� �񵿱� ���� ����� OverlappedTask�� ���ϴ� �Լ��̴�.
	return m_TaskQueue->NetworkBind(m_ListenSocket,
	                                [this](auto&& returnType, auto&& NumberOfBytes, auto&& OverlappedPtr)
	                                {
		                                OnAccept(std::forward<decltype(returnType)>(returnType)
											, std::forward<decltype(NumberOfBytes)>(NumberOfBytes)
											, std::forward<decltype(OverlappedPtr)>(OverlappedPtr));
	                                });
}

// backlog ������ŭ ���Ӵ��
bool TCPListener::StartAccept(int backlog)
{
	// 128���� �Ѱ�� �д�
	if (0 > backlog || 128 < backlog)
	{
		SYSTEM_INFO system_info;
		GetSystemInfo(&system_info);

		backlog = system_info.dwNumberOfProcessors;
	}

	for (int i = 0; i < backlog; ++i)
	{
		AsyncAccept();
	}

	return true;
}

void TCPListener::BroadCast(const std::vector<unsigned char>& data, const std::shared_ptr<TCPSession>& ignore_session)
{
	std::lock_guard lock(m_ConnectionLock);
	for (auto& session : m_Connections)
	{
		if (ignore_session == session.second)
		{
			continue;
		}

		session.second->Send(data);
	}
}

void TCPListener::OnAccept(BOOL result, DWORD byte_size, LPOVERLAPPED overlapped)
{
	// ���Ӵ���� �� ���� ���Ͽ����� �Ǿ�
	// ������ ������� ���ο� ���Ӵ�� ������ �����Ѵ�
	AsyncAccept();

	if (nullptr == overlapped)
		return;

	if (nullptr == m_AcceptCallBack)
		return;

	// �Ű������� �Ѿ�� overlapped�� ���� �ּҿ� �����Լ����̺� ũ��(8����Ʈ) ��ŭ ���־�
	// overlapped�� AcceptExOverlapped Ŭ������ �����Ͽ� ����Ѵ�.
	std::unique_ptr<AcceptExOverlapped> overlappedPtr = 
		std::unique_ptr<AcceptExOverlapped>(reinterpret_cast<AcceptExOverlapped*>(reinterpret_cast<char*>(overlapped) - sizeof(void*)));

	if (0 != result)
	{
		overlappedPtr->Execute(TRUE, byte_size);

		// �̹� Bind�� Socket�� ��Ȱ��Ǿ� OnAccept �Լ��� ������ ��
		// IOCP�� �ٽ� Bind ���� �ʵ��� �����ش�
		if (false == overlappedPtr->GetTCPSession()->m_IsAcceptBind)
		{
			// GameServerQueue == m_TaskQueue�� ������ �ִ� IOCP�� ���ø� �޵��� �Ͽ�
			// SessionSocket���� ���� ���� �߻��� �� TCPSession::OnCallBack() �Լ��� ȣ���ϵ��� ����
			overlappedPtr->GetTCPSession()->BindQueue(*m_TaskQueue);
			overlappedPtr->GetTCPSession()->AcceptBindOn();
		}

		// TCPListener Initializer �� �����صξ��� AcceptCallBack ȣ�� -> GameServerApp::main()�� ���� ������ �Լ�
		// TCPSession�� ���� ȣ���� �̷�����Ƿ� AcceptCallBack�� ȣ���ϱ� ������ TCPSession�� GameServerQueue�� Bind �Ǿ�� �Ѵ�.
		// -> ȣ�� ������ Ʋ���� ���� �߻�(��. SendOverlapped�� ���� leak)
		m_AcceptCallBack(std::static_pointer_cast<TCPSession>(overlappedPtr->GetTCPSession()->shared_from_this()));

		// IOCP�� ���ø� �޴� SessionSocket���� ���� ���� �߻����� ��
		// �� ���𰡰� � �Ϳ��� �߻����� ���� ���� ==> ���� Receive ����
		overlappedPtr->GetTCPSession()->RecvRequest();

		m_ConnectionLock.lock();
		m_Connections.insert(make_pair(overlappedPtr->GetTCPSession()->GetConnectId(), overlappedPtr->GetTCPSession()));
		m_ConnectionLock.unlock();

		// IOCP�� �����ϴ� AcceptExOverlapped �޸𸮸�
		// �����ڿ� ����� ������ �����ϰ� �Ǿ����Ƿ�
		// ���� Pool���� ������ AcceptExOverlappedPool�� �־��ش�
		// ����, overlappedPtr(AcceptExOverlapped)�� unique_ptr�̹Ƿ� release ���ش�.
		m_IocpAcceptExOverlappedPool.Erase(overlappedPtr.get());
		m_AcceptExOverlappedPool.Push(overlappedPtr.get());
		overlappedPtr.release();
	}
	else
	{
		GameServerDebug::LogError("Accept Error");
	}
}

void TCPListener::AsyncAccept()
{
	// ���Ӵ�� ���� �����Լ�
	// 10���� ���Ӵ�� ������ �����ΰ�
	// ���� �Ѹ�� ���Ͽ����� �ϰԵǸ� �ش� ������ �ش� ������ ������ ����
	// ��ԵǴ� �ڸ��� �ٽ� AsyncAccept ȣ���� ���� �����

	// �ִ� ������ 20�̶�� �� ��,
	// ����� ���ϰ� ������� ���� ������ �� 20���� �Ǹ� �� �̻� ������ ������ �ʴ´�.

	// ���� �� ���� Disconnect�� ���
	// �ش� ������ Destroy�ϰ� ������ ���� ����� ���� �ƴ϶�
	// ������ ������ �ش� ������ m_ConnectionPool�� ��� ��Ȱ���Ѵ�.
	// �����찡 ������� �� �ִ� ������ �Ѱ�ġ�� �ֱ⿡ ���� ����� ���� �ƴ� ��Ȱ���� �ϴ� ���̴�.
	// ���� ��⿭�� ������ ��ٸ��� ������ �ִٸ� �� ������ ��Ȱ���Ͽ� �ٽ� ��������ش�.


	// ������ �ڵ鸵�� �޸� ���������� �������ִ� ���� �ƴϴ�.
	// �����带 10���� �����ξ��ٸ�, ���ÿ� 10���� �����尡 ��� ������ �� �ִ�.
	// �׷��⿡ 10���� �����尡 ���ÿ� AsyncAccept �Լ��� ȣ���ϸ� ������ �߻��� �� �ִ�.
	PtrSTCPSession newSession = nullptr;
	{
		std::lock_guard lock(m_ConnectionPoolLock);

		// ��Ȱ�뿡 ����� Ǯ�� ����ִٸ�
		if (m_ConnectionPool.empty())
		{
			newSession = std::make_unique<TCPSession>();
			newSession->Initialize();
			newSession->SetParent(this);

			std::string logText = std::to_string(static_cast<int>(newSession->GetSocket()));
			GameServerDebug::LogInfo(logText + " Created New Socket");
		}
		else
		{
			newSession = m_ConnectionPool.front();
			m_ConnectionPool.pop_front();

			std::string logText = std::to_string(static_cast<int>(newSession->GetSocket()));
			GameServerDebug::LogInfo(logText + " Socket Recycled");
		}
	}

	AcceptExOverlapped* acceptOverlapped = nullptr;
	if (true == m_AcceptExOverlappedPool.IsEmpty())
	{
		acceptOverlapped = new AcceptExOverlapped(newSession);
	}
	else
	{
		acceptOverlapped = m_AcceptExOverlappedPool.Pop();
		acceptOverlapped->SetTCPSession(newSession);
	}
	DWORD dwByte = 0;

	// Overlapped�� ���� �ּҸ� �Ѱ��־���
	// �ش� Overlapped �ּҴ� PostQueued -> GetQueued�� ����
	// GameServerQueue�� Work �Լ��� �ִ� OverTask�� task function���� ���ϰ�
	// �ش� task function�� BindQueue �Լ��� ���� ���� OnAccept �Լ��� ����
	// OnAccept �Լ����� ���� Overlapped�� �ּҿ��� �����Լ����̺� ��ŭ�� �ּ���ġ�� ����
	// AcceptExOverlapped ��ü�� ������ �� �ִ�.

	/*
	 * AcceptEx(sListenSocket, sAcceptSocket, lpOutputBuffer, dwReceiveDataLength,
	 *			dwLocalAddressLength, dwRemoteAddressLength, lpdwBytesReceived, lpOverlapped)
	 *
	 * AcceptEx�� ������ Ŭ���̾�Ʈ�� ������ �޾Ƶ��� �� �񵿱� ȣ��(asynchronous call)�� �����ϰ� �Ͽ�
	 * �ٸ� Ŭ���̾�Ʈ�� ������ ��� �޾Ƶ��� �� �ֵ��� �Ѵ�.
	 *
	 * @param
	 * In  SOCKET		sListenSocket			: ���� ��� �Լ��� �̹� ȣ��� ������ �ĺ��ϴ� ������
	 *											  ���� ���α׷��� �� ���Ͽ��� ���� �õ��� ��ٸ���
	 *
	 * In  SOCKET		sAcceptSocket			: ������ ������ ����� ������ �ĺ��ϴ� ������
	 *											  �� ������ ���ε��ǰų� ����Ǹ� �ȵȴ�
	 *
	 * In  PVOID		lpOutputBuffer			: �� ���ῡ�� ���۵� ù ��° ������ ���
	 *											  ������ ���� �ּ� �� Ŭ���̾�Ʈ�� ���� �ּҸ� �����ϴ� ���ۿ� ���� ������
	 *											  ���� �����ʹ� ������ 0���� �����Ͽ� ������ ù ��° �κп� ��ϵǴ� �ݸ� �ּҴ� ������ �޺κп� ��ϵȴ�
	 *											  �� �Ű������� �ݵ�� �����ؾ� �Ѵ�
	 *
	 * In  DWORD		dwReceiveDataLength		: ���� ���� �����Ϳ� ���� lpOutputBuffer�� ����Ʈ ��
	 *											  �� ũ�⿡�� ������ ���� �ּҳ� Ŭ���̾�Ʈ�� ���� �ּҰ� ���Ե��� �ʾƾ� �ϸ� ��� ���ۿ� �߰��ȴ�
	 *											  dwReceiveDataLength�� 0�� ��� ������ �����ص� �����۾��� ������� �ʴ´�
	 *											  ��� AcceptEx�� �����͸� ��ٸ��� �ʰ� ������ �����ϴ� ��� �Ϸ�ȴ� == �񵿱� �۾�
	 *
	 * In  DWORD		dwLocalAddressLength	: ���� �ּ� ������ ����� ����Ʈ ��
	 *											  �� ���� ��� ���� ���� ���������� �ִ� �ּ� ���̺��� 16����Ʈ �̻� Ŀ���Ѵ�
	 *
	 * In  DWORD		dwRemoteAddressLength	: ���� �ּ� ������ ����� ����Ʈ ��
	 *											  �� ���� ��� ���� ���� ���������� �ִ� �ּ� ���̺��� 16����Ʈ �̻� Ŀ���Ѵ�
	 *											  ����, ����Ʈ ���� 0�� �� ����
	 *
	 * Out LPDWORD		lpdwBytesReceived		: ���ŵ� ����Ʈ ���� �����ϴ� DWORD�� ���� ������
	 *											  �� �Ű������� �۾��� ���������� �Ϸ�� ��쿡�� �����ȴ�
	 *											  ERROR_IO_PENDING�� ��ȯ�� ��� �� DWORD�� �������� ������
	 *											  �Ϸ� �˸� ��Ŀ���򿡼� ���� ����Ʈ ���� ���;� �Ѵ�
	 *
	 * In  LPOVERLAPPED lpOverlapped			: ��û�� ó���ϴµ� ���Ǵ� OVERLAPPED ����ü
	 *											  �� �Ű������� �����Ǿ� �ϸ� NULL�� �� ����
	 */
	const BOOL result = AcceptEx(m_ListenSocket,
		newSession->GetSocket(),
		acceptOverlapped->GetBuffer(),
		0,
		sizeof(sockaddr_in) + 16,
		sizeof(sockaddr_in) + 16,
		&dwByte,
		acceptOverlapped->GetOverlapped());

	if (FALSE == result)
	{
		if (ERROR_IO_PENDING != WSAGetLastError())
		{
			GameServerDebug::GetLastErrorPrint();
			return;
		}
	}

	m_IocpAcceptExOverlappedPool.Push(acceptOverlapped);
}

void TCPListener::CloseSession(const PtrSTCPSession& tcp_session)
{
	// CloseSession �Լ����� ���� �� ������
	// ������� ������ �Ϸ�� ����
	{
		// ���� ���� ���� ���ǿ� ���� hash_map���� ������ �Ǿ�����
		// CloseSession�� ���� tcpSession�� �ش� �ʿ��� �����ش�
		std::lock_guard lock(m_ConnectionLock);
		m_Connections.erase(tcp_session->GetConnectId());
	}

	{
		// Disconnect�� Socket�� �������� ���� �ƴ�
		// ��Ȱ���� ���� ConnectionPool�� ��Ƶд�
		std::lock_guard lock(m_ConnectionPoolLock);
		m_ConnectionPool.push_back(tcp_session);
	}
}

void TCPListener::Close()
{
	if (NULL != m_ListenSocket)
	{
		closesocket(m_ListenSocket);
		m_ListenSocket = NULL;
	}
}