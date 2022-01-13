#pragma once
#include "IPEndPoint.h"

// �뵵 : 
// �з� :
// ÷�� : 
class TCPSession : public std::enable_shared_from_this<TCPSession>
{
private: // Member Var

public: // Default
	TCPSession();
	~TCPSession();

	TCPSession(const TCPSession& other) = delete;
	TCPSession(TCPSession&& other) noexcept;

public:
	TCPSession& operator=(const TCPSession& other) = delete;
	TCPSession& operator=(TCPSession&& other) = delete;

private:

public: // Member Function
};

