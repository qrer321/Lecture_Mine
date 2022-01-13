#include "PreCompile.h"
#include "TCPListener.h"
#include "TCPSession.h"
#include "ServerHelper.h"
#include "SocketAddress.h"
#include "IPEndPoint.h"

TCPListener::TCPListener()
	: m_ListenSocket(NULL)
	, m_TaskQueue(nullptr)
{
}

TCPListener::~TCPListener()
= default;

TCPListener::TCPListener(TCPListener&& other) noexcept
	: m_ListenSocket(other.m_ListenSocket)
	, m_TaskQueue(other.m_TaskQueue)
{

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
	return m_TaskQueue->NetworkBind(m_ListenSocket, std::bind(&TCPListener::OnAccept, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

void TCPListener::OnAccept(BOOL, DWORD, LPOVERLAPPED)
{
}

void TCPListener::Close()
{
	if (NULL != m_ListenSocket)
	{
		closesocket(m_ListenSocket);
		m_ListenSocket = NULL;
	}
}

