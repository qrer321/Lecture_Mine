#pragma once
#include "Enums.h"
#include "EndPoint.h"
#include "IPAddress.h"

// 용도 : 
// 분류 :
// 첨언 :
class SocketAddress;
class IPEndPoint : public EndPoint
{
private: // Member Var
	IPAddress	m_Address;
	int			m_Port;

public: // Default
	IPEndPoint();
	~IPEndPoint() override;

	IPEndPoint(__int64 address, int port);
	IPEndPoint(const IPEndPoint& other);
	IPEndPoint(const IPAddress& other, int port);

public:
	IPEndPoint& operator=(const IPEndPoint& other);
	bool operator==(const IPEndPoint& other) const;
	bool operator!=(const IPEndPoint& other) const;

public:
	AddressFamily GetAddressFamily() const override;
	SocketAddress Serialize() const override;
	uint64_t GetHashCode() const override;


public: // Member Function
	IPAddress GetAddress() const;
	int GetPort() const;
};

