#pragma once
#include "TypeDefine.h"

// 용도 : 
// 분류 :
// 첨언 : 
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

class AcceptExOverlapped : public GameServerOverlapped
{
private:
	char			m_Buffer[128];
	PtrSTCPSession	m_TCPSession;

public:
	AcceptExOverlapped() = delete;
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
	void Execute(BOOL result, DWORD byteSize) override;
};

class RecvOverlapped : public GameServerOverlapped
{
private:
	char			m_Buffer[1024];
	WSABUF			m_wsaBuffer;
	PtrSTCPSession	m_TCPSession;

public:
	RecvOverlapped() = delete;
	RecvOverlapped(PtrSTCPSession tcpSession);
	~RecvOverlapped() override;

	RecvOverlapped(const RecvOverlapped& other) = delete;
	RecvOverlapped(RecvOverlapped&& other) = delete;

public:
	RecvOverlapped& operator=(const RecvOverlapped& other) = delete;
	RecvOverlapped& operator=(RecvOverlapped&& other) = delete;


public:
	PtrSTCPSession GetTCPSession() { return m_TCPSession; }
	void* GetBuffer() { return m_Buffer; }
	LPWSABUF GetWSABuffer() { return &m_wsaBuffer;  }

public:
	void Execute(BOOL result, DWORD byteSize) override;
	void Clear();
};

class DisconnectOverlapped : public GameServerOverlapped
{
private:
	PtrSTCPSession	m_TCPSession;

public:
	DisconnectOverlapped() = delete;
	DisconnectOverlapped(PtrSTCPSession tcpSession);
	~DisconnectOverlapped() override;

	DisconnectOverlapped(const DisconnectOverlapped& other) = delete;
	DisconnectOverlapped(DisconnectOverlapped&& other) = delete;

public:
	DisconnectOverlapped& operator=(const DisconnectOverlapped& other) = delete;
	DisconnectOverlapped& operator=(DisconnectOverlapped&& other) = delete;

public:
	void Execute(BOOL result, DWORD byteSize) override;
};