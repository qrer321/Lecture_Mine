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
	, m_CallBack(nullptr)
{
}

TCPListener::~TCPListener()
= default;

TCPListener::TCPListener(TCPListener&& other) noexcept
	: m_ListenSocket(other.m_ListenSocket)
	, m_TaskQueue(other.m_TaskQueue)
{
	other.m_ListenSocket = NULL;
	other.m_TaskQueue = nullptr;
}

bool TCPListener::Initialize(const IPEndPoint& endPoint, const std::function<void(std::shared_ptr<TCPSession>)>& callback)
{
	// WSAStartUp
	ServerHelper::StartEngineStartUp();

	// �ּҿ� ��� ���� ��Ŷ���� ������ ���� ����Ʈ ������ ����ȭ�Ѵ�.
	SocketAddress address = endPoint.Serialize();

	// �񵿱� �׼�Ʈ�� �Ϸ��� ���� ����
	m_ListenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, NULL, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET == m_ListenSocket)
	{
		GameServerDebug::GetLastErrorPrint();
		return false;
	}
	
	int error = ::bind(m_ListenSocket, reinterpret_cast<const sockaddr*>(address.GetBuffer()), static_cast<int>(address.GetBufferLength()));
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

	m_ListenEndPoint = endPoint;
	m_CallBack = callback;

	return true;
}

bool TCPListener::BindQueue(const GameServerQueue& taskQueue)
{
	m_TaskQueue = &taskQueue;

	// ���ܳ��� �Լ��� ����
	// NetworkBind�� �񵿱� ���� ����� OverlappedTask�� ���ϴ� �Լ��̴�.
	return m_TaskQueue->NetworkBind(m_ListenSocket, std::bind(&TCPListener::OnAccept, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

void TCPListener::OnAccept(IocpWaitReturnType result, DWORD byteSize, LPOVERLAPPED overlapped)
{
	AcceptExOverlapped* acceptOver = reinterpret_cast<AcceptExOverlapped*>(reinterpret_cast<char*>(overlapped) - sizeof(void*));
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

	for (size_t i = 0; i < backlog; ++i)
	{
		AsyncAccept();
	}

	return true;
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
		m_ConnectLock.lock();

		// ��Ȱ�뿡 ����� Ǯ�� ����ִٸ�
		if (m_ConnectionPool.empty())
		{
			newSession = std::make_shared<TCPSession>();
			newSession->Initialize();
			newSession->SetParent(this);
		}
		else
		{
			newSession = m_ConnectionPool.front();
			m_ConnectionPool.pop_front();
		}

		m_ConnectLock.unlock();
	}

	std::unique_ptr<AcceptExOverlapped> acceptOver = std::make_unique<AcceptExOverlapped>(newSession);

	DWORD dwByte = 0;

	// Overlapped�� ���� �ּҸ� �Ѱ��־���
	// �ش� Overlapped �ּҴ� PostQueued -> GetQueued�� ����
	// GameServerQueue�� Work �Լ��� �ִ� OverTask�� task function���� ���ϰ�
	// �ش� task function�� BindQueue �Լ��� ���� ���� OnAccept �Լ��� ����
	// OnAccept �Լ����� ���� Overlapped�� �ּҿ��� �����Լ����̺� ��ŭ�� �ּ���ġ�� ����
	// AcceptExOverlapped ��ü�� ������ �� �ִ�.
	const BOOL result = AcceptEx(m_ListenSocket, newSession->GetSocket(), acceptOver->GetBuffer(),
		0, sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16,
		&dwByte, acceptOver->GetOverlapped());

	if (FALSE == result)
	{
		if (ERROR_IO_PENDING != WSAGetLastError())
		{
			GameServerDebug::GetLastErrorPrint();
			return;
		}
	}

	acceptOver.release();
}

void TCPListener::Close()
{
	if (NULL != m_ListenSocket)
	{
		closesocket(m_ListenSocket);
		m_ListenSocket = NULL;
	}
}

