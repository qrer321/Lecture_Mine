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
= default;

TCPListener::TCPListener(TCPListener&& other) noexcept
	: m_ListenSocket(other.m_ListenSocket)
	, m_TaskQueue(other.m_TaskQueue)
	, m_AcceptCallBack(std::move(other.m_AcceptCallBack))
{
	other.m_ListenSocket = INVALID_SOCKET;
	other.m_TaskQueue = nullptr;
}

bool TCPListener::Initialize(const IPEndPoint& endPoint, const std::function<void(std::shared_ptr<TCPSession>)>& acceptCallback)
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
	m_AcceptCallBack = acceptCallback;

	return true;
}

bool TCPListener::BindQueue(const GameServerQueue& taskQueue)
{
	m_TaskQueue = &taskQueue;

	// 숨겨놓은 함수로 연결
	// NetworkBind는 비동기 파일 입출력 OverlappedTask로 통하는 함수이다.
	return m_TaskQueue->NetworkBind(m_ListenSocket,
	                                [this](auto&& returnType, auto&& NumberOfBytes, auto&& OverlappedPtr)
	                                {
		                                OnAccept(std::forward<decltype(returnType)>(returnType)
											, std::forward<decltype(NumberOfBytes)>(NumberOfBytes)
											, std::forward<decltype(OverlappedPtr)>(OverlappedPtr));
	                                });
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

	for (int i = 0; i < backlog; ++i)
	{
		AsyncAccept();
	}

	return true;
}

void TCPListener::OnAccept(BOOL result, DWORD byteSize, LPOVERLAPPED overlapped)
{
	// 접속대기자 한 명이 소켓연결이 되어
	// 소켓을 들고갔으니 새로운 접속대기 소켓을 생성한다
	AsyncAccept();

	if (nullptr == overlapped)
		return;

	if (nullptr == m_AcceptCallBack)
		return;

	// 매개변수로 넘어온 overlapped에 대한 주소에 가상함수테이블 크기(8바이트) 만큼 빼주어
	// overlapped의 AcceptExOverlapped 클래스를 복구하여 사용한다.
	std::unique_ptr<AcceptExOverlapped> overlappedPtr = 
		std::unique_ptr<AcceptExOverlapped>(reinterpret_cast<AcceptExOverlapped*>(reinterpret_cast<char*>(overlapped) - sizeof(void*)));

	if (0 != result)
	{
		overlappedPtr->Execute(TRUE, byteSize);

		m_AcceptCallBack(overlappedPtr->GetTCPSession());

		// Receive를 위한 바인드
		overlappedPtr->GetTCPSession()->BindQueue(*m_TaskQueue);
		overlappedPtr->GetTCPSession()->RecvRequest();

		m_ConnectionLock.lock();
		m_Connections.insert(make_pair(overlappedPtr->GetTCPSession()->GetConnectId(), overlappedPtr->GetTCPSession()));
		m_ConnectionLock.unlock();
	}
	else
	{
		GameServerDebug::LogError("Accept Error");
	}
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
		m_ConnectionPoolLock.lock();

		// 재활용에 사용할 풀이 비어있다면
		if (m_ConnectionPool.empty())
		{
			newSession = std::make_shared<TCPSession>();
			newSession->Initialize();
			newSession->SetParent(this);

			HANDLE eventHandle = newSession->m_DisconnectOverlapped->GetOverlapped()->hEvent;

			std::string logText = std::to_string(static_cast<int>(newSession->GetSocket()));
			GameServerDebug::LogInfo(logText + " 소켓을 새로 생성하였습니다");
		}
		else
		{
			newSession = m_ConnectionPool.front();
			m_ConnectionPool.pop_front();

			HANDLE eventHandle = newSession->m_DisconnectOverlapped->GetOverlapped()->hEvent;

			std::string logText = std::to_string(static_cast<int>(newSession->GetSocket()));
			GameServerDebug::LogInfo(logText + " 소켓을 재활용하여 새로운 대기 소켓을 만들었습니다");
		}

		m_ConnectionPoolLock.unlock();
	}

	std::unique_ptr<AcceptExOverlapped> overlappedPtr = std::make_unique<AcceptExOverlapped>(newSession);

	DWORD dwByte = 0;

	// Overlapped에 대한 주소를 넘겨주었다
	// 해당 Overlapped 주소는 PostQueued -> GetQueued를 거쳐
	// GameServerQueue의 Work 함수에 있는 OverTask의 task function으로 통하고
	// 해당 task function은 BindQueue 함수로 인해 묶인 OnAccept 함수로 간다
	// OnAccept 함수에서 받은 Overlapped의 주소에서 가상함수테이블 만큼의 주소위치를 빼면
	// AcceptExOverlapped 객체를 복구할 수 있다.

	/*
	 * AcceptEx(sListenSocket, sAcceptSocket, lpOutputBuffer, dwReceiveDataLength,
	 *			dwLocalAddressLength, dwRemoteAddressLength, lpdwBytesReceived, lpOverlapped)
	 *
	 * AcceptEx는 서버가 클라이언트의 연결을 받아들일 때 비동기 호출(asynchronous call)로 동작하게 하여
	 * 다른 클라이언트의 연결을 즉시 받아들일 수 있도록 한다.
	 *
	 * @param
	 * In  SOCKET		sListenSocket			: 수신 대기 함수로 이미 호출된 소켓을 식별하는 설명자
	 *											  서버 프로그램이 이 소켓에서 연결 시도를 기다린다
	 *
	 * In  SOCKET		sAcceptSocket			: 들어오는 연결을 허용할 소켓을 식별하는 설명자
	 *											  이 소켓은 바인딩되거나 연결되면 안된다
	 *
	 * In  PVOID		lpOutputBuffer			: 새 연결에서 전송된 첫 번째 데이터 블록
	 *											  서버의 로컬 주소 및 클라이언트의 원격 주소를 수신하는 버퍼에 대한 포인터
	 *											  수신 데이터는 오프셋 0에서 시작하여 버퍼의 첫 번째 부분에 기록되는 반면 주소는 버퍼의 뒷부분에 기록된다
	 *											  이 매개변수는 반드시 지정해야 한다
	 *
	 * In  DWORD		dwReceiveDataLength		: 실제 수신 데이터에 사용될 lpOutputBuffer의 바이트 수
	 *											  이 크기에는 서버의 로컬 주소나 클라이언트의 원격 주소가 포함되지 않아야 하며 출력 버퍼에 추가된다
	 *											  dwReceiveDataLength가 0인 경우 연결을 승인해도 수신작업이 수행되지 않는다
	 *											  대신 AcceptEx는 데이터를 기다리지 않고 연결이 도착하는 즉시 완료된다 == 비동기 작업
	 *
	 * In  DWORD		dwLocalAddressLength	: 로컬 주소 정보에 예약된 바이트 수
	 *											  이 값은 사용 중인 전송 프로토콜의 최대 주소 길이보다 16바이트 이상 커야한다
	 *
	 * In  DWORD		dwRemoteAddressLength	: 원격 주소 정보에 예약된 바이트 수
	 *											  이 값은 사용 중인 전송 프로토콜의 최대 주소 길이보다 16바이트 이상 커야한다
	 *											  또한, 바이트 수는 0일 수 없다
	 *
	 * Out LPDWORD		lpdwBytesReceived		: 수신된 바이트 수를 수신하는 DWORD에 대한 포인터
	 *											  이 매개변수는 작업이 동기적으로 완료될 경우에만 설정된다
	 *											  ERROR_IO_PENDING이 반환될 경우 이 DWORD는 설정되지 않으며
	 *											  완료 알림 메커니즘에서 읽은 바이트 수를 얻어와야 한다
	 *
	 * In  LPOVERLAPPED lpOverlapped			: 요청을 처리하는데 사용되는 OVERLAPPED 구조체
	 *											  이 매개변수는 지정되야 하며 NULL일 수 없다
	 */
	const BOOL result = AcceptEx(m_ListenSocket,
		newSession->GetSocket(),
		overlappedPtr->GetBuffer(),
		0,
		sizeof(sockaddr_in) + 16,
		sizeof(sockaddr_in) + 16,
		&dwByte,
		overlappedPtr->GetOverlapped());

	if (FALSE == result)
	{
		if (ERROR_IO_PENDING != WSAGetLastError())
		{
			GameServerDebug::GetLastErrorPrint();
			return;
		}
	}

	overlappedPtr.release();
}

void TCPListener::CloseSession(const PtrSTCPSession& tcpSession)
{
	// CloseSession 함수까지 오게 된 세션은
	// 연결끊기 동작이 완료된 소켓
	{
		// 현재 연결 중인 세션에 대한 hash_map에서 빠지게 되었으니
		// CloseSession에 들어온 tcpSession을 해당 맵에서 지워준다
		std::lock_guard lock(m_ConnectionLock);
		m_Connections.erase(tcpSession->GetConnectId());
	}

	{
		// Disconnect된 Socket은 지워지는 것이 아닌
		// 재활용을 위해 ConnectionPool에 담아둔다
		std::lock_guard lock(m_ConnectionPoolLock);
		m_ConnectionPool.push_back(tcpSession);
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