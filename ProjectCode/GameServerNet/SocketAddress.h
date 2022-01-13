#pragma once
#include "Enums.h"

// 용도 : 
// 분류 :
// 첨언 :
class IPEndPoint;
class IPAddress;
class SocketAddress
{
private: // Member Var
	char*	m_Buffer;
	size_t	m_BufferMaxSize;
	size_t	m_BufferLength;

public: // Default
	SocketAddress();
	SocketAddress(const SocketAddress& other);
	SocketAddress(SocketAddress&& other) noexcept;
	SocketAddress(AddressFamily addressFamily);
	SocketAddress(const IPAddress& address);
	SocketAddress(const IPAddress& address, int port);
	~SocketAddress();

public:
	SocketAddress& operator=(const SocketAddress& other);
	SocketAddress& operator=(SocketAddress&& other) noexcept;

private:
	void Initialize();
	void Destroy();

public: // Member Function
	const char* GetBuffer() const;
	size_t GetBufferLength() const;

public:
	AddressFamily GetAddressFamily() const;

	int GetPort() const;
	IPAddress GetIPAddress() const;
	IPEndPoint GetIPEndPoint() const;
};

