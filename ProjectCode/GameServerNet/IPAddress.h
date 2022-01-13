#pragma once
#include "Enums.h"

// 용도 : 
// 분류 :
// 첨언 : 
class IPAddress
{
private: // Member Var
	bool			m_InvalidAddress;
	__int64			m_Address;
	AddressFamily	m_AddressFamily;

public: // Default
	IPAddress() = delete;
	IPAddress(__int64 newAddress);
	IPAddress(const IPAddress& other) noexcept;
	IPAddress(IPAddress&& other) noexcept;
	~IPAddress();

public:
	IPAddress& operator=(const IPAddress& other) noexcept;
	IPAddress& operator=(IPAddress&& other) noexcept;

public: // Member Function
	static IPAddress Parse(const std::string& ip);

	AddressFamily GetAddressFamily() const;
	__int64 Address() const;
};

