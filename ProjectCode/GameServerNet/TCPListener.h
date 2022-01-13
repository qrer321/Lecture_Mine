#pragma once

// 용도 : 
// 분류 :
// 첨언 : 
class TCPListener
{
private: // Member Var

public: // Default
	TCPListener();
	~TCPListener();

	TCPListener(const TCPListener& other) = delete;
	TCPListener(TCPListener&& other) noexcept;

public:
	TCPListener& operator=(const TCPListener& other) = delete;
	TCPListener& operator=(TCPListener&& other) = delete;

private:

public: // Member Function
};

