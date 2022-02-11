#pragma once
#include "IPEndPoint.h"
#include "TypeDefine.h"
#include <GameServerBase/GameServerObjectBase.h>
#include <GameServerBase/GameServerObjectPool.h>
#include <GameServerBase/GameServerQueue.h>

// �뵵 : 
// �з� :
// ÷�� :
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
	bool BindQueue(const GameServerQueue& taskQueue);	// nullptr�� ���� �� ������ �Ű������� &�� �޴´�
	void AcceptBindOn();								// �̹� Bind�� Socket�� �ٽñ� Bind���� �ʵ��� ���� �Լ�

	void OnSendComplete(SendOverlapped* sendOverlapped);
	void OnRecv(const char* data, DWORD byteSize);
	void RecvRequest();
		
	void Close(bool forceClose = false);
	void DisconnectSocket();	// ���� ���� �� ��Ȱ��
	void CloseSocket();			// ���� ����

public:
	SOCKET	GetSocket()		const { return m_SessionSocket; }
	__int64 GetConnectId()	const { return m_ConnectId; }

public: // Member Function
	bool Send(const std::vector<unsigned char>& buffer);

	void SetCallBack(const RecvCallBack& recvCallBack, const CloseCallBack& closeCallBack);
	void UnRegisterSession();
};

