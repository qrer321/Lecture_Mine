#pragma once

// �뵵 : 
// �з� :
// ÷�� : 
class SocketBase : public GameServerNameBase
{
private: // Member Var

public: // Default
	SocketBase();
	~SocketBase();

	SocketBase(const SocketBase& other) = delete;
	SocketBase(SocketBase&& other) noexcept;

public:
	SocketBase& operator=(const SocketBase& other) = delete;
	SocketBase& operator=(SocketBase&& other) = delete;

private:

public: // Member Function
};

