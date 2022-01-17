#pragma once
#include "SocketBase.h"
#include "IPEndPoint.h"
#include "TypeDefine.h"
#include <deque>

// 용도 : 
// 분류 :
// 첨언 :
class TCPSession;
class GameServerQueue;
class TCPListener : public SocketBase
{
private:
	SOCKET										m_ListenSocket;
	IPEndPoint									m_ListenEndPoint;

	/*
	 * GameServerQueue 내부의 IOCP에 m_ListenSocket을 등록시키고
	 * IOCP를 이용하여 접속자 이벤트를 처리한다.
	 */
	const GameServerQueue*						m_TaskQueue;

	std::function<void(PtrSTCPSession)>			m_CallBack;
	std::mutex									m_ConnectLock;

	/*
	 * 미리 n개의 세션을 만들어둔다.
	 * AcceptEx를 호출하면 호출한 횟수만큼 백로그가 생기는 시스템.
	 */
	std::deque<PtrSTCPSession>					m_ConnectionPool;

	std::unordered_map<__int64, PtrSTCPSession>	m_Connections;

public: // Default
	TCPListener();
	~TCPListener() override;

	TCPListener(const TCPListener& other) = delete;
	TCPListener(TCPListener&& other) noexcept;

public:
	TCPListener& operator=(const TCPListener& other) = delete;
	TCPListener& operator=(TCPListener&& other) = delete;

private:
	void Close() override;

public: // Member Function
	bool Initialize(const IPEndPoint& endPoint, const std::function<void(PtrSTCPSession)>& callback);
	bool BindQueue(const GameServerQueue& taskQueue);
	bool StartAccept(int backlog);

	void OnAccept(IocpWaitReturnType result, DWORD byteSize, LPOVERLAPPED overlapped);

protected:
	void AsyncAccept();
};

