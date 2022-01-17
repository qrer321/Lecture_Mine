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

	// 주소에 담긴 값을 패킷으로 보내기 위해 바이트 단위로 직렬화한다.
	SocketAddress address = endPoint.Serialize();

	// 비동기 액셉트를 하려는 소켓 설정
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

	// m_ListenSocket 옵션 설정
	BOOL on = TRUE;
	error = setsockopt(m_ListenSocket, SOL_SOCKET, SO_CONDITIONAL_ACCEPT, reinterpret_cast<const char*>(&on), sizeof(on));
	if (SOCKET_ERROR == error)
	{
		GameServerDebug::GetLastErrorPrint();
		Close();
		return false;
	}

	// 백로그 개수 설정
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

	// 숨겨놓은 함수로 연결
	// NetworkBind는 비동기 파일 입출력 OverlappedTask로 통하는 함수이다.
	return m_TaskQueue->NetworkBind(m_ListenSocket, std::bind(&TCPListener::OnAccept, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

void TCPListener::OnAccept(IocpWaitReturnType result, DWORD byteSize, LPOVERLAPPED overlapped)
{
	AcceptExOverlapped* acceptOver = reinterpret_cast<AcceptExOverlapped*>(reinterpret_cast<char*>(overlapped) - sizeof(void*));
}

// backlog 개수만큼 접속대기
bool TCPListener::StartAccept(int backlog)
{
	// 128명을 한계로 둔다
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
	// 접속대기 소켓 생성함수
	// 10개의 접속대기 소켓을 만들어두고
	// 유저 한명과 소켓연결을 하게되면 해당 소켓은 해당 유저가 가지고 간다
	// 비게되는 자리는 다시 AsyncAccept 호출을 통해 만든다

	// 최대 동접을 20이라고 볼 때,
	// 연결된 소켓과 연결되지 않은 소켓이 총 20개가 되면 더 이상 소켓은 만들지 않는다.

	// 유저 한 명이 Disconnect한 경우
	// 해당 소켓을 Destroy하고 소켓을 새로 만드는 것이 아니라
	// 연결이 해제된 해당 소켓을 m_ConnectionPool에 담아 재활용한다.
	// 윈도우가 만들어줄 수 있는 소켓의 한계치가 있기에 새로 만드는 것이 아닌 재활용을 하는 것이다.
	// 만약 대기열에 접속을 기다리는 유저가 있다면 그 소켓을 재활용하여 다시 연결시켜준다.


	// 스레드 핸들링은 메모리 동시접근을 제어해주는 것이 아니다.
	// 스레드를 10개를 만들어두었다면, 동시에 10개의 스레드가 깨어나 동작할 수 있다.
	// 그렇기에 10개의 스레드가 동시에 AsyncAccept 함수를 호출하면 문제가 발생할 수 있다.
	PtrSTCPSession newSession = nullptr;
	{
		m_ConnectLock.lock();

		// 재활용에 사용할 풀이 비어있다면
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

	// Overlapped에 대한 주소를 넘겨주었다
	// 해당 Overlapped 주소는 PostQueued -> GetQueued를 거쳐
	// GameServerQueue의 Work 함수에 있는 OverTask의 task function으로 통하고
	// 해당 task function은 BindQueue 함수로 인해 묶인 OnAccept 함수로 간다
	// OnAccept 함수에서 받은 Overlapped의 주소에서 가상함수테이블 만큼의 주소위치를 빼면
	// AcceptExOverlapped 객체를 복구할 수 있다.
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

