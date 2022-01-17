#pragma once
#include "SocketBase.h"
#include "IPEndPoint.h"
#include "TypeDefine.h"
#include <deque>

// �뵵 : 
// �з� :
// ÷�� :
class TCPSession;
class GameServerQueue;
class TCPListener : public SocketBase
{
private:
	SOCKET										m_ListenSocket;
	IPEndPoint									m_ListenEndPoint;

	/*
	 * GameServerQueue ������ IOCP�� m_ListenSocket�� ��Ͻ�Ű��
	 * IOCP�� �̿��Ͽ� ������ �̺�Ʈ�� ó���Ѵ�.
	 */
	const GameServerQueue*						m_TaskQueue;

	std::function<void(PtrSTCPSession)>			m_CallBack;
	std::mutex									m_ConnectLock;

	/*
	 * �̸� n���� ������ �����д�.
	 * AcceptEx�� ȣ���ϸ� ȣ���� Ƚ����ŭ ��αװ� ����� �ý���.
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

