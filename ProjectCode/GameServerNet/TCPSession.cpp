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

long long TCPSession::GetConnectId() const
{
	return m_ConnectId;
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

	return true;
}

bool TCPSession::BindQueue(const GameServerQueue& taskQueue)
{

	

	return true;
}

void TCPSession::Close()
{
}
