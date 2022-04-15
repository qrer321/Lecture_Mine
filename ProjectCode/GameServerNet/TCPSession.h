#pragma once
#include <GameServerBase/GameServerObjectBase.h>
#include <GameServerBase/GameServerObjectPool.h>
#include <GameServerBase/GameServerQueue.h>
#include "IPEndPoint.h"
#include "TypeDefine.h"

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

	uint64_t	m_ConnectId;
	IPEndPoint	m_LocalAddress;
	IPEndPoint	m_RemoteAddress;
	bool		m_IsAcceptBind;

	GameServerObjectPool<SendOverlapped>	m_SendOverlappedPool;
	RecvOverlapped*							m_RecvOverlapped;
	DisconnectOverlapped*					m_DisconnectOverlapped;

	std::function<void(PtrSTCPSession, const std::vector<unsigned char>&)>	m_RecvCallBack;
	std::function<void(PtrSTCPSession)>										m_CloseCallBack;
	bool																	m_CallClose;

public: // Default
	TCPSession();
	~TCPSession() override;

	TCPSession(const TCPSession& other) = delete;
	TCPSession(TCPSession&& other) noexcept;

public:
	TCPSession& operator=(const TCPSession& other) = delete;
	TCPSession& operator=(TCPSession&& other) = delete;

private:
	static void OnCallBack(BOOL result, DWORD number_of_bytes, LPOVERLAPPED overlapped);

	bool Initialize();
	bool BindQueue(const GameServerQueue& task_queue);	// nullptr가 들어올 수 없도록 매개변수를 &로 받는다
	void AcceptBindOn() { m_IsAcceptBind = true; }		// 이미 Bind된 Socket이 다시금 Bind되지 않도록 막는 함수

	void OnSendComplete(SendOverlapped* send_overlapped);
	void OnRecv(const char* recv_buffer, DWORD number_of_bytes);
	void Recv();
		
	void Close(bool force_close = false);
	void DisconnectSocket();	// 정상 종료 후 재활용
	void CloseSocket();			// 강제 종료

public:
	SOCKET	 GetSocket()	const { return m_SessionSocket; }
	uint64_t GetConnectId()	const { return m_ConnectId; }

	bool Send(const std::vector<unsigned char>& send_data);

	void SetCallBack(const std::function<void(PtrSTCPSession, const std::vector<unsigned char>&)>& recv_callback, const std::function<void(PtrSTCPSession)>& close_callback);
	void SetRecvCallBack(const std::function<void(PtrSTCPSession, const std::vector<unsigned char>&)>& callback);
	void SetCloseCallBack(const std::function<void(PtrSTCPSession)>& callback);

	void UnRegisterSession();
};

