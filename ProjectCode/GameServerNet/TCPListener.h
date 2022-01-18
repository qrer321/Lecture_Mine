#pragma once
#include "IPEndPoint.h"
#include "TypeDefine.h"
#include <deque>

// �뵵 : 
// �з� :
// ÷�� :
class TCPSession;
class GameServerQueue;
class TCPListener : public GameServerObjectBase
{
private:
	SOCKET										m_ListenSocket;
	IPEndPoint									m_ListenEndPoint;

	/*
	 * GameServerQueue ������ IOCP�� m_ListenSocket�� ��Ͻ�Ű��
	 * IOCP�� �̿��Ͽ� ������ �̺�Ʈ�� ó���Ѵ�.
	 */
	const GameServerQueue*						m_TaskQueue;

	std::function<void(PtrSTCPSession)>			m_AcceptCallBack;
	

	/*
	 * �̸� n���� ������ �����д�.
	 * AcceptEx�� ȣ���ϸ� ȣ���� Ƚ����ŭ ��αװ� ����� �ý���.
	 */
	std::deque<PtrSTCPSession>					m_ConnectionPool;
	std::mutex									m_ConnectPoolLock;

	// __int64 : ConnectionId
	std::unordered_map<__int64, PtrSTCPSession>	m_Connections;
	std::mutex									m_ConnectionLock;

public: // Default
	TCPListener();
	~TCPListener() override;

	TCPListener(const TCPListener& other) = delete;
	TCPListener(TCPListener&& other) noexcept;

public:
	TCPListener& operator=(const TCPListener& other) = delete;
	TCPListener& operator=(TCPListener&& other) = delete;

private:

public: // Member Function
	bool Initialize(const IPEndPoint& endPoint, const std::function<void(PtrSTCPSession)>& acceptCallback);
	bool BindQueue(const GameServerQueue& taskQueue);
	bool StartAccept(int backlog);

	void OnAccept(IocpWaitReturnType result, DWORD byteSize, LPOVERLAPPED overlapped);

	void Close();

protected:
	void AsyncAccept();
};

