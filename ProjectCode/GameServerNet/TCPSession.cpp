#include "PreCompile.h"
#include "TCPSession.h"

TCPSession::TCPSession()
	: m_SessionSocket(NULL)
{}

TCPSession::~TCPSession()
= default;

TCPSession::TCPSession(TCPSession&& other) noexcept
	: m_SessionSocket(other.m_SessionSocket)
{
	other.m_SessionSocket = NULL;
}

SOCKET TCPSession::GetSocket() const
{
	return m_SessionSocket;
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

	return true;
}

void TCPSession::Close()
{
}