#pragma once
#include "IPEndPoint.h"
#include "TypeDefine.h"
#include <deque>

// 용도 : 
// 분류 :
// 첨언 :
class TCPSession;
class GameServerQueue;
class AcceptExOverlapped;
class TCPListener : public GameServerObjectBase
{
	friend TCPSession;

private:
	SOCKET											m_ListenSocket;
	IPEndPoint										m_ListenEndPoint;

	
	// GameServerQueue 내부의 IOCP에 m_ListenSocket을 등록시키고
	// IOCP를 이용하여 접속자 이벤트를 처리한다.
	const GameServerQueue*							m_TaskQueue;
	std::function<void(PtrSTCPSession)>				m_AcceptCallBack;

	// IOCP에 맡겨져있는 AcceptExOverlapped의 메모리들
	GameServerObjectFindPool<AcceptExOverlapped>	m_IocpAcceptExOverlappedPool;
	GameServerObjectPool<AcceptExOverlapped>		m_AcceptExOverlappedPool;

	// 미리 n개의 세션을 만들어둔다.
	// AcceptEx를 호출하면 호출한 횟수만큼 백로그가 생기는 시스템.
	std::deque<PtrSTCPSession>						m_ConnectionPool;
	std::mutex										m_ConnectionPoolLock;

	// __int64 : ConnectionId
	std::unordered_map<__int64, PtrSTCPSession>		m_Connections;
	std::mutex										m_ConnectionLock;

public: // Default
	TCPListener();
	~TCPListener() override;

	TCPListener(const TCPListener& other) = delete;
	TCPListener(TCPListener&& other) noexcept;

public:
	TCPListener& operator=(const TCPListener& other) = delete;
	TCPListener& operator=(TCPListener&& other) = delete;

private:
	void OnAccept(BOOL result, DWORD byte_size, LPOVERLAPPED overlapped);
	void AsyncAccept();

	void CloseSession(const PtrSTCPSession& tcp_session);

public: // Member Function
	bool Initialize(const IPEndPoint& end_point, const std::function<void(PtrSTCPSession)>& accept_callback);
	bool BindQueue(const GameServerQueue& task_queue);
	bool StartAccept(int backlog);

	void BroadCast(const std::vector<unsigned char>& data, const std::shared_ptr<TCPSession>& ignore_session);

	void Close();
};

