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
	SOCKET														m_ListenSocket;
	IPEndPoint													m_ListenEndPoint;

	/*
	 * GameServerQueue ������ IOCP�� m_ListenSocket�� ��Ͻ�Ű��
	 * IOCP�� �̿��Ͽ� ������ �̺�Ʈ�� ó���Ѵ�.
	 */
	const GameServerQueue*										m_TaskQueue;
	std::function<void(std::shared_ptr<TCPSession>)>			m_CallBack;
	std::mutex													m_ConnectLock;

	/*
	 * �̸� n���� ������ �����д�.
	 * AcceptEx�� ȣ���ϸ� ȣ���� Ƚ����ŭ ��αװ� ����� �ý���.
	 */
	std::deque<std::shared_ptr<TCPSession>>						m_ConnectionPool;
	std::unordered_map<__int64, std::shared_ptr<TCPSession>>	m_Connections;

public: // Default
	TCPListener();
	~TCPListener() override;

	TCPListener(const TCPListener& other) = delete;
	TCPListener(TCPListener&& other) noexcept;

public:
	TCPListener& operator=(const TCPListener& other) = delete;
	TCPListener& operator=(TCPListener&& other) = delete;

public: // Member Function
	bool Initialize(const IPEndPoint& endPoint, const std::function<void(std::shared_ptr<TCPSession>)>& callback);
	bool BindQueue(const GameServerQueue& taskQueue);

	void OnAccept(BOOL, DWORD, LPOVERLAPPED);

	void Close();
};

