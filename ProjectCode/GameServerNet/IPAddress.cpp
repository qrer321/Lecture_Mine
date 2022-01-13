#include "PreCompile.h"
#include "IPAddress.h"


IPAddress::IPAddress(__int64 newAddress)
	: m_InvalidAddress(newAddress < 0 || newAddress > 0x00000000FFFFFFFF)
	, m_Address(newAddress)
	, m_AddressFamily(AddressFamily::IPv4)
{
}

IPAddress::IPAddress(const IPAddress& other) noexcept
	: m_InvalidAddress(other.m_InvalidAddress)
	, m_Address(other.m_Address)
	, m_AddressFamily(other.m_AddressFamily)
{
}

IPAddress::IPAddress(IPAddress&& other) noexcept
	: m_InvalidAddress(other.m_InvalidAddress)
	, m_Address(other.m_Address)
	, m_AddressFamily(other.m_AddressFamily)
{
}

IPAddress::~IPAddress()
= default;

IPAddress& IPAddress::operator=(const IPAddress& other) noexcept
{
	m_InvalidAddress = other.m_InvalidAddress;
	m_Address = other.m_Address;
	m_AddressFamily = other.m_AddressFamily;

	return *this;
}

IPAddress& IPAddress::operator=(IPAddress&& other) noexcept
{
	m_InvalidAddress = other.m_InvalidAddress;
	m_Address = other.m_Address;
	m_AddressFamily = other.m_AddressFamily;

	other.m_InvalidAddress = false;
	other.m_Address = 0;
	other.m_AddressFamily = AddressFamily::Unknown;

	return *this;
}

IPAddress IPAddress::Parse(const std::string& ip)
{
	if (true == ip.empty())
		return IPAddress(0);

	SOCKADDR_IN add;
	add.sin_family = AF_INET;

	if (ip == "localhost")
		inet_pton(AF_INET, "127.0.0.1", &add.sin_addr.s_addr);
	else if (ip == "0.0.0.0")
		add.sin_addr.s_addr = htonl(INADDR_ANY);
	else
		inet_pton(AF_INET, ip.c_str(), &add.sin_addr.s_addr);

	return IPAddress(add.sin_addr.s_addr);
}

AddressFamily IPAddress::GetAddressFamily() const
{
	return m_AddressFamily;
}

__int64 IPAddress::Address() const
{
	return m_Address;
}
