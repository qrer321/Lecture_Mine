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
	virtual ~GameServerOverlapped() = default;

public: // Member Function
	LPWSAOVERLAPPED GetOverlapped();
	void ResetOverlapped();
};

class AcceptExOverlapped : public GameServerOverlapped
{
private:
	char m_Buffer[128];
	std::shared_ptr<TCPSession> m_TCPSession;

public:
	AcceptExOverlapped() = delete;
	AcceptExOverlapped(const PtrSTCPSession& tcpSession);
	~AcceptExOverlapped();

public:
	void Execute(BOOL result, DWORD byteSize);

	void* GetBuffer() { return m_Buffer; }
	PtrSTCPSession GetTCPSession() { return m_TCPSession; }
};