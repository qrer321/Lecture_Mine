#pragma once

// 용도 : 
// 분류 :
// 첨언 : 
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
	virtual void Close() {}

public: // Member Function
};

