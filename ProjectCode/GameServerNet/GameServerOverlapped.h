#pragma once
#include "TypeDefine.h"

// 용도 : 
// 분류 :
// 첨언 :

/////////////////////// GameServerOverlapped ///////////////////////
class GameServerOverlapped
{
protected: // Member Var
	WSAOVERLAPPED m_Overlapped;

public: // Default
	GameServerOverlapped();
	virtual ~GameServerOverlapped() = 0;

	GameServerOverlapped(const GameServerOverlapped& other) = delete;
	GameServerOverlapped(GameServerOverlapped&& other) = delete;

public:
	GameServerOverlapped& operator=(const GameServerOverlapped& other) = delete;
	GameServerOverlapped& operator=(GameServerOverlapped&& other) = delete;

public: // Member Function
	LPWSAOVERLAPPED GetOverlapped();
	void ResetOverlapped();

	virtual void Execute(BOOL result, DWORD byteSize) {}
};


/////////////////////// AcceptExOverlapped ///////////////////////
class AcceptExOverlapped : public GameServerOverlapped
{
private:
	char			m_Buffer[128];
	PtrSTCPSession	m_TCPSession;

public:
	AcceptExOverlapped();
	AcceptExOverlapped(PtrSTCPSession tcpSession);
	~AcceptExOverlapped() override;

	AcceptExOverlapped(const AcceptExOverlapped& other) = delete;
	AcceptExOverlapped(AcceptExOverlapped&& other) = delete;

public:
	AcceptExOverlapped& operator=(const AcceptExOverlapped& other) = delete;
	AcceptExOverlapped& operator=(AcceptExOverlapped&& other) = delete;

public:
	void* GetBuffer() { return m_Buffer; }
	PtrSTCPSession GetTCPSession() { return m_TCPSession; }

public:
	void SetTCPSession(PtrSTCPSession tcpSession);
	void Execute(BOOL result, DWORD byteSize) override;
};


/////////////////////// SendOverlapped ///////////////////////
class SendOverlapped : public GameServerOverlapped
{
private:
	std::vector<char>	m_Buffer;
	WSABUF				m_wsaBuffer;
	TCPSession*			m_TCPSession;

public:
	SendOverlapped();
	SendOverlapped(TCPSession* tcpSession);
	~SendOverlapped() override;

	SendOverlapped(const SendOverlapped& other) = delete;
	SendOverlapped(SendOverlapped&& other) = delete;

public:
	SendOverlapped& operator=(const SendOverlapped& other) = delete;
	SendOverlapped& operator=(SendOverlapped&& other) = delete;

public:
	LPWSABUF GetBuffer();
	int GetMaxBufferLength() const;

public:
	void SetTCPSession(TCPSession* tcpSession);
	void New(size_t maxBufferLength);
	void CopyFrom(const std::vector<char>& from);

	void Execute(BOOL result, DWORD byteSize) override;
};


/////////////////////// RecvOverlapped ///////////////////////
class RecvOverlapped : public GameServerOverlapped
{
private:
	char			m_Buffer[1024];
	WSABUF			m_wsaBuffer;
	TCPSession*		m_TCPSession;

public:
	RecvOverlapped() = delete;
	RecvOverlapped(TCPSession* tcpSession);
	~RecvOverlapped() override;

	RecvOverlapped(const RecvOverlapped& other) = delete;
	RecvOverlapped(RecvOverlapped&& other) = delete;

public:
	RecvOverlapped& operator=(const RecvOverlapped& other) = delete;
	RecvOverlapped& operator=(RecvOverlapped&& other) = delete;


public:
	TCPSession* GetTCPSession() const { return m_TCPSession; }
	void* GetBuffer() { return m_Buffer; }
	LPWSABUF GetWSABuffer() { return &m_wsaBuffer;  }

public:
	void Execute(BOOL result, DWORD byteSize) override;
	void Clear();
};


/////////////////////// DisconnectOverlapped ///////////////////////
class DisconnectOverlapped : public GameServerOverlapped
{
private:
	TCPSession* m_TCPSession;

public:
	DisconnectOverlapped() = delete;
	DisconnectOverlapped(TCPSession* tcpSession);
	~DisconnectOverlapped() override;

	DisconnectOverlapped(const DisconnectOverlapped& other) = delete;
	DisconnectOverlapped(DisconnectOverlapped&& other) = delete;

public:
	DisconnectOverlapped& operator=(const DisconnectOverlapped& other) = delete;
	DisconnectOverlapped& operator=(DisconnectOverlapped&& other) = delete;

public:
	void Execute(BOOL result, DWORD byteSize) override;
};