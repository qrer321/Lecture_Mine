#include "PreCompile.h"
#include "TCPSession.h"
#include "GameServerOverlapped.h"

TCPSession::TCPSession()
	: m_SessionSocket(NULL)
	, m_ConnectId(0)
	, m_RecvOverlapped(nullptr)
	, m_CallClose(false)
{}

TCPSession::~TCPSession()
{
	delete m_RecvOverlapped;
}

TCPSession::TCPSession(TCPSession&& other) noexcept
	: m_SessionSocket(other.m_SessionSocket)
	, m_ConnectId(other.m_ConnectId)
	, m_RecvOverlapped(other.m_RecvOverlapped)
	, m_CallClose(other.m_CallClose)
{
	other.m_RecvOverlapped = nullptr;
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
		m_RecvOverlapped = new RecvOverlapped(std::dynamic_pointer_cast<TCPSession>(shared_from_this()));
	}

	return true;
}

bool TCPSession::BindQueue(const GameServerQueue& taskQueue)
{
	PtrWTCPSession sessionPtr = std::dynamic_pointer_cast<TCPSession>(shared_from_this());
	return taskQueue.NetworkBind(m_SessionSocket, 
		std::bind(&TCPSession::OnCallBack, sessionPtr, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

void TCPSession::OnCallBack(PtrWTCPSession weakSession, BOOL result, DWORD numberOfBytes, LPOVERLAPPED lpOverlapped)
{
	if (nullptr == lpOverlapped)
		return;

	auto overlappedPtr = reinterpret_cast<GameServerOverlapped*>(reinterpret_cast<char*>(lpOverlapped) - sizeof(void*));

	// recv던 send던 다형성을 통해 적절한 클래스의 Execute 함수 호출
	overlappedPtr->Execute(result, numberOfBytes);
}

void TCPSession::OnRecv(const char* data, DWORD byteSize)
{
	if (0 == byteSize)
	{
		Close();
		return;
	}

	// 프로토콜 처리
	// byteSize만큼 vector resize
	std::vector<char> outputBuffer = std::vector<char>(byteSize);
	memcpy_s(&outputBuffer[0], byteSize, data, byteSize);

	if (nullptr != m_RecvCallBack)
	{
		m_RecvCallBack(std::dynamic_pointer_cast<TCPSession>(shared_from_this()), outputBuffer);
	}

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

void TCPSession::SetCallBack(const RecvCallBack& recvCallBack, const CloseCallBack& closeCallBack)
{
	if (nullptr == recvCallBack)
	{
		GameServerDebug::LogError("ReceiveCallBack Is NULL");
	}

	m_RecvCallBack = recvCallBack;
	m_CloseCallBack = closeCallBack;
}

void TCPSession::Close()
{
	if (nullptr != m_CloseCallBack)
	{
		m_CloseCallBack(std::dynamic_pointer_cast<TCPSession>(shared_from_this()));
	}

	m_CallClose = true;

	CloseSocket();
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