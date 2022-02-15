#include "PreCompile.h"
#include "IPEndPoint.h"
#include "SocketAddress.h"

IPEndPoint::IPEndPoint()
	: m_Address(0)
	, m_Port(0)
{
}

IPEndPoint::~IPEndPoint()
= default;

IPEndPoint::IPEndPoint(long long address, int port)
	: m_Address(address)
	, m_Port(port)
{
}

IPEndPoint::IPEndPoint(const IPEndPoint& other)
	: m_Address(other.m_Address)
	, m_Port(other.m_Port)
{
}

IPEndPoint::IPEndPoint(const IPAddress& other, int port)
	: m_Address(other)
	, m_Port(port)
{
}

IPEndPoint& IPEndPoint::operator=(const IPEndPoint& other)
{
	m_Port = other.m_Port;
	m_Address = other.m_Address;

	return *this;
}

bool IPEndPoint::operator==(const IPEndPoint& other) const
{
	return other.m_Address.Address() == m_Address.Address() && other.m_Port == m_Port;
}

bool IPEndPoint::operator!=(const IPEndPoint& other) const
{
	return !(*this == other);
}

AddressFamily IPEndPoint::GetAddressFamily() const
{
	return m_Address.GetAddressFamily();
}

SocketAddress IPEndPoint::Serialize() const
{
	return { m_Address, m_Port };
}

uint64_t IPEndPoint::GetHashCode() const
{
	return (m_Address.Address() << 16) | m_Port;
}

IPAddress IPEndPoint::GetAddress() const
{
	return m_Address;
}

int IPEndPoint::GetPort() const
{
	return m_Port;
}

