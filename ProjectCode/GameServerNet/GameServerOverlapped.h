#pragma once
#include <utility>

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

	virtual void Execute(BOOL result, DWORD number_of_bytes) {}
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
	~AcceptExOverlapped() override = default;

	AcceptExOverlapped(const AcceptExOverlapped& other) = delete;
	AcceptExOverlapped(AcceptExOverlapped&& other) = delete;

public:
	AcceptExOverlapped& operator=(const AcceptExOverlapped& other) = delete;
	AcceptExOverlapped& operator=(AcceptExOverlapped&& other) = delete;

public:
	void* GetBuffer() { return m_Buffer; }
	PtrSTCPSession GetTCPSession() { return m_TCPSession; }

public:
	void SetTCPSession(PtrSTCPSession tcp_session);
	void Execute(BOOL result, DWORD number_of_bytes) override;
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
	SendOverlapped(TCPSession* tcp_session);
	~SendOverlapped() override = default;

	SendOverlapped(const SendOverlapped& other) = delete;
	SendOverlapped(SendOverlapped&& other) = delete;

public:
	SendOverlapped& operator=(const SendOverlapped& other) = delete;
	SendOverlapped& operator=(SendOverlapped&& other) = delete;

public:
	LPWSABUF GetBuffer();
	int GetMaxBufferLength() const;

public:
	void SetTCPSession(TCPSession* tcp_session);
	void New(size_t max_buffer_length);
	void CopyFrom(const std::vector<unsigned char>& from);

	void Execute(BOOL result, DWORD number_of_bytes) override;
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
	RecvOverlapped(TCPSession* tcp_session);
	~RecvOverlapped() override = default;

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
	void Execute(BOOL result, DWORD number_of_bytes) override;
	void Clear();
};


/////////////////////// DisconnectOverlapped ///////////////////////
class DisconnectOverlapped : public GameServerOverlapped
{
private:
	TCPSession* m_TCPSession;

public:
	DisconnectOverlapped() = delete;
	DisconnectOverlapped(TCPSession* tcp_session);
	~DisconnectOverlapped() override = default;

	DisconnectOverlapped(const DisconnectOverlapped& other) = delete;
	DisconnectOverlapped(DisconnectOverlapped&& other) = delete;

public:
	DisconnectOverlapped& operator=(const DisconnectOverlapped& other) = delete;
	DisconnectOverlapped& operator=(DisconnectOverlapped&& other) = delete;

public:
	void Execute(BOOL result, DWORD number_of_bytes) override;
};



/////////////////////// UDPRecvOverlapped ///////////////////////
class UDPSession;
class UDPRecvOverlapped : public GameServerOverlapped
{
private:
	std::weak_ptr<UDPSession>	m_UDPSession;
	char						m_Buffer[1024]{};
	WSABUF						m_wsaBuffer{};
	DWORD						m_NumberOfBytes{};

public:
	UDPRecvOverlapped(std::weak_ptr<UDPSession> udp_session);
	~UDPRecvOverlapped() override = default;

	UDPRecvOverlapped(const UDPRecvOverlapped& other) = delete;
	UDPRecvOverlapped(UDPRecvOverlapped&& other) = delete;
	UDPRecvOverlapped& operator=(const UDPRecvOverlapped& other) = delete;
	UDPRecvOverlapped& operator=(UDPRecvOverlapped&& other) = delete;

public:
	void*	 GetBuffer()		{ return m_Buffer; }
	LPDWORD	 GetBufferLength()	{ return &m_NumberOfBytes; }
	LPWSABUF GetWSABuffer()		{ return &m_wsaBuffer; }

	void Execute(BOOL result, DWORD number_of_bytes) override;
};



/////////////////////// UDPSendOverlapped ///////////////////////
class UDPSession;
class UDPSendOverlapped : public GameServerOverlapped
{
private:
	std::weak_ptr<UDPSession>	m_UDPSession;
	char						m_Buffer[1024]{};
	WSABUF						m_wsaBuffer{};
	DWORD						m_NumberOfBytes{};

public:
	UDPSendOverlapped() = default;
	~UDPSendOverlapped() override = default;

	UDPSendOverlapped(const UDPSendOverlapped& other) = delete;
	UDPSendOverlapped(UDPSendOverlapped&& other) = delete;
	UDPSendOverlapped& operator=(const UDPSendOverlapped& other) = delete;
	UDPSendOverlapped& operator=(UDPSendOverlapped&& other) = delete;

public:
	void*	 GetBuffer()		{ return m_Buffer; }
	LPDWORD	 GetBufferLength()	{ return &m_NumberOfBytes; }
	LPWSABUF GetWSABuffer()		{ return &m_wsaBuffer; }

	void SetUDPSession(std::weak_ptr<UDPSession> udp_session) { m_UDPSession = std::move(udp_session); }

	void Execute(BOOL result, DWORD number_of_bytes) override;
	void CopyFrom(const std::vector<unsigned char>& from_data);
};