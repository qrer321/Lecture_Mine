#pragma once
#include "IPEndPoint.h"

// 용도 : 
// 분류 :
// 첨언 :
class TCPListener;
class AcceptExOverlapped;
class TCPSession : public GameServerObjectBase
{
	friend TCPListener;
	friend AcceptExOverlapped;

private: // Member Var
	SOCKET m_SessionSocket;

	__int64 m_ConnectId;
	IPEndPoint m_LocalAddress;
	IPEndPoint m_RemoteAddress;

public: // Default
	TCPSession();
	~TCPSession() override;

	TCPSession(const TCPSession& other) = delete;
	TCPSession(TCPSession&& other) noexcept;

public:
	TCPSession& operator=(const TCPSession& other) = delete;
	TCPSession& operator=(TCPSession&& other) = delete;

private:
	void Close();

public:
	SOCKET GetSocket() const;
	__int64 GetConnectId() const;

public: // Member Function
	bool Initialize();
	bool BindQueue(const GameServerQueue& taskQueue);	// nullptr가 들어올 수 없도록 매개변수를 &로 받는다
};

