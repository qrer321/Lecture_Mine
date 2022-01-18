#pragma once
#include "IPEndPoint.h"
#include "TypeDefine.h"

// 용도 : 
// 분류 :
// 첨언 :
class TCPListener;
class AcceptExOverlapped;
class RecvOverlapped;
class TCPSession : public GameServerObjectBase
{
	friend TCPListener;
	friend AcceptExOverlapped;
	friend RecvOverlapped;

private: // Member Var
	SOCKET		m_SessionSocket;

	__int64		m_ConnectId;
	IPEndPoint	m_LocalAddress;
	IPEndPoint	m_RemoteAddress;

	RecvOverlapped* m_RecvOverlapped;

	bool m_CallClose;
	std::function<void(PtrSTCPSession)>								m_CloseCallBack;
	std::function<void(PtrSTCPSession, const std::vector<char>&)>	m_RecvCallBack;

	using CloseCallBack = std::function<void(PtrSTCPSession)>;
	using RecvCallBack = std::function<void(PtrSTCPSession, const std::vector<char>&)>;

public: // Default
	TCPSession();
	~TCPSession() override;

	TCPSession(const TCPSession& other) = delete;
	TCPSession(TCPSession&& other) noexcept;

public:
	TCPSession& operator=(const TCPSession& other) = delete;
	TCPSession& operator=(TCPSession&& other) = delete;

private:
	SOCKET GetSocket() const { return m_SessionSocket; }

private:
	bool Initialize();
	bool BindQueue(const GameServerQueue& taskQueue);	// nullptr가 들어올 수 없도록 매개변수를 &로 받는다

	static void OnCallBack(PtrWTCPSession weakSession, BOOL result, DWORD numberOfBytes, LPOVERLAPPED lpOverlapped);

	void OnRecv(const char* data, DWORD byteSize);
	void RecvRequest();

	void Close();
	void CloseSocket();

public:
	__int64 GetConnectId() const { return m_ConnectId; }

public: // Member Function
	void SetCallBack(const RecvCallBack& recvCallBack, const CloseCallBack& closeCallBack);
};

