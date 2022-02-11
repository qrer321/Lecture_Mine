#pragma once
#include "IPEndPoint.h"
#include "TypeDefine.h"
#include <deque>

// �뵵 : 
// �з� :
// ÷�� :
class TCPSession;
class GameServerQueue;
class AcceptExOverlapped;
class TCPListener : public GameServerObjectBase
{
	friend TCPSession;

private:
	SOCKET											m_ListenSocket;
	IPEndPoint										m_ListenEndPoint;

	
	// GameServerQueue ������ IOCP�� m_ListenSocket�� ��Ͻ�Ű��
	// IOCP�� �̿��Ͽ� ������ �̺�Ʈ�� ó���Ѵ�.
	const GameServerQueue*							m_TaskQueue;
	std::function<void(PtrSTCPSession)>				m_AcceptCallBack;

	// IOCP�� �ð����ִ� AcceptExOverlapped�� �޸𸮵�
	GameServerObjectFindPool<AcceptExOverlapped>	m_IocpAcceptExOverlappedPool;
	GameServerObjectPool<AcceptExOverlapped>		m_AcceptExOverlappedPool;

	// �̸� n���� ������ �����д�.
	// AcceptEx�� ȣ���ϸ� ȣ���� Ƚ����ŭ ��αװ� ����� �ý���.
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

