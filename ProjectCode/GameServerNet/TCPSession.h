#pragma once
#include "SocketBase.h"

// �뵵 : 
// �з� :
// ÷�� : 
class TCPSession : public SocketBase
{
private: // Member Var
	SOCKET m_SessionSocket;

public: // Default
	TCPSession();
	~TCPSession() override;

	TCPSession(const TCPSession& other) = delete;
	TCPSession(TCPSession&& other) noexcept;

public:
	TCPSession& operator=(const TCPSession& other) = delete;
	TCPSession& operator=(TCPSession&& other) = delete;

private:
	void Close() override;

public: // Member Function
	SOCKET GetSocket() const;

	bool Initialize();
};

