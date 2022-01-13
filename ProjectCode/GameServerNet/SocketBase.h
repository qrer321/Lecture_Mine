#pragma once

// �뵵 : 
// �з� :
// ÷�� : 
class SocketBase
{
private: // Member Var
	SOCKET m_Socket;

public: // Default
	SocketBase();
	~SocketBase();

	SocketBase(const SocketBase& other) = delete;
	SocketBase(SocketBase&& other) noexcept;

public:
	SocketBase& operator=(const SocketBase& other) = delete;
	SocketBase& operator=(SocketBase&& other) = delete;

protected:
	void Close();

public: // Member Function
};

