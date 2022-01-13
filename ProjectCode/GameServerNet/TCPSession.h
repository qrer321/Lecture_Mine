#pragma once
#include "IPEndPoint.h"

// 용도 : 
// 분류 :
// 첨언 : 
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

