#pragma once
#include "IPEndPoint.h"
#include "TypeDefine.h"
#include <GameServerBase/GameServerObjectBase.h>
#include <GameServerBase/GameServerObjectPool.h>
#include <GameServerBase/GameServerQueue.h>

// 용도 : 
// 분류 :
// 첨언 :
class TCPListener;
class AcceptExOverlapped;
class SendOverlapped;
class RecvOverlapped;
class DisconnectOverlapped;
class TCPSession : public GameServerObjectBase
{
	friend TCPListener;
	friend AcceptExOverlapped;
	friend SendOverlapped;
	friend RecvOverlapped;

private: // Member Var
	SOCKET		m_SessionSocket;

	__int64		m_ConnectId;
	IPEndPoint	m_LocalAddress;
	IPEndPoint	m_RemoteAddress;
	bool		m_IsAcceptBind;

	GameServerObjectPool<SendOverlapped>	m_SendOverlappedPool;
	RecvOverlapped*							m_RecvOverlapped;
	DisconnectOverlapped*					m_DisconnectOverlapped;

	std::function<void(PtrSTCPSession, const std::vector<unsigned char>&)>	m_RecvCallBack;
	std::function<void(PtrSTCPSession)>										m_CloseCallBack;
	bool																	m_CallClose;

	using RecvCallBack = std::function<void(PtrSTCPSession, const std::vector<unsigned char>&)>;
	using CloseCallBack = std::function<void(PtrSTCPSession)>;

public: // Default
	TCPSession();
	~TCPSession() override;

	TCPSession(const TCPSession& other) = delete;
	TCPSession(TCPSession&& other) noexcept;

public:
	TCPSession& operator=(const TCPSession& other) = delete;
	TCPSession& operator=(TCPSession&& other) = delete;

private:
	static void OnCallBack(PtrWTCPSession weakSession, BOOL result, DWORD numberOfBytes, LPOVERLAPPED lpOverlapped);

	bool Initialize();
	bool BindQueue(const GameServerQueue& taskQueue);	// nullptr가 들어올 수 없도록 매개변수를 &로 받는다
	void AcceptBindOn();								// 이미 Bind된 Socket이 다시금 Bind되지 않도록 막는 함수

	void OnSendComplete(SendOverlapped* sendOverlapped);
	void OnRecv(const char* data, DWORD byteSize);
	void RecvRequest();
		
	void Close(bool forceClose = false);
	void DisconnectSocket();	// 정상 종료 후 재활용
	void CloseSocket();			// 강제 종료

public:
	SOCKET	GetSocket()		const { return m_SessionSocket; }
	__int64 GetConnectId()	const { return m_ConnectId; }

public: // Member Function
	bool Send(const std::vector<unsigned char>& buffer);

	void SetCallBack(const RecvCallBack& recvCallBack, const CloseCallBack& closeCallBack);
	void UnRegisterSession();
};

