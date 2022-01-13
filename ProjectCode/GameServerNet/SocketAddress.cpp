#include "PreCompile.h"
#include "SocketAddress.h"
#include "IPAddress.h"
#include "IPEndPoint.h"

SocketAddress::SocketAddress()
	: m_Buffer(nullptr)
	, m_BufferMaxSize(0)
	, m_BufferLength(0)
{

}

SocketAddress::SocketAddress(const SocketAddress& other)
	: m_Buffer(nullptr)
	, m_BufferMaxSize(0)
	, m_BufferLength(0)
{
	if (nullptr != other.m_Buffer)
	{
		Initialize();

		m_BufferLength = other.m_BufferLength;
		memcpy_s(m_Buffer, m_BufferMaxSize, other.m_Buffer, other.m_BufferLength);
	}
}

SocketAddress::SocketAddress(SocketAddress&& other) noexcept
	: m_Buffer(other.m_Buffer)
	, m_BufferMaxSize(other.m_BufferMaxSize)
	, m_BufferLength(other.m_BufferLength)
{
	other.m_Buffer = nullptr;
	other.m_BufferMaxSize = 0;
	other.m_BufferLength = 0;
}

SocketAddress::SocketAddress(AddressFamily addressFamily)
	: m_Buffer(nullptr)
	, m_BufferMaxSize(0)
	, m_BufferLength(sizeof(SOCKADDR_IN6))
{
	Initialize();

	if (AddressFamily::IPv4 == addressFamily)
		m_BufferLength = sizeof(SOCKADDR_IN);

	int family = static_cast<int>(addressFamily);

	m_Buffer[0] = static_cast<char>(family);
	m_Buffer[1] = static_cast<char>((family >> 8));
}

SocketAddress::SocketAddress(const IPAddress& address)
	: SocketAddress(address.GetAddressFamily())
{
	m_Buffer[2] = 0;
	m_Buffer[3] = 0;

	__int64 ipAddress = address.Address();

	m_Buffer[4] = static_cast<char>(ipAddress);
	m_Buffer[5] = static_cast<char>((ipAddress >> 8));
	m_Buffer[6] = static_cast<char>((ipAddress >> 16));
	m_Buffer[7] = static_cast<char>((ipAddress >> 24));
}

SocketAddress::SocketAddress(const IPAddress& address, int port)
	: SocketAddress(address)
{
	m_Buffer[2] = static_cast<char>((port >> 8));
	m_Buffer[3] = static_cast<char>(port);
}

SocketAddress::~SocketAddress()
{
	Destroy();
}

SocketAddress& SocketAddress::operator=(const SocketAddress& other)
{
	if (nullptr != other.m_Buffer)
	{
		Initialize();

		m_BufferLength = other.m_BufferLength;
		memcpy_s(m_Buffer , m_BufferMaxSize, other.m_Buffer, other.m_BufferLength);
	}

	return *this;
}

SocketAddress& SocketAddress::operator=(SocketAddress&& other) noexcept
{
	Destroy();

	m_Buffer = other.m_Buffer;
	m_BufferMaxSize = other.m_BufferMaxSize;
	m_BufferLength = other.m_BufferLength;

	other.m_Buffer = nullptr;
	other.m_BufferMaxSize = 0;
	other.m_BufferLength = 0;

	return *this;
}

void SocketAddress::Initialize()
{
	if (nullptr == m_Buffer)
	{
		m_BufferMaxSize = sizeof(SOCKADDR_IN) < sizeof(SOCKADDR_IN6) ? sizeof(SOCKADDR_IN6) : sizeof(SOCKADDR_IN);
		m_Buffer = new char[m_BufferMaxSize];
	}

	m_BufferLength = 0;
	memset(m_Buffer, 0x00, m_BufferMaxSize);
}

void SocketAddress::Destroy()
{
	if (nullptr != m_Buffer)
	{
		delete[] m_Buffer;
		m_Buffer = nullptr;
		m_BufferMaxSize = 0;
		m_BufferLength = 0;
	}
}

const char* SocketAddress::GetBuffer() const
{
	return m_Buffer;
}

size_t SocketAddress::GetBufferLength() const
{
	return m_BufferLength;
}

AddressFamily SocketAddress::GetAddressFamily() const
{
	int family = m_Buffer[0] | (m_Buffer[1] << 8);
	return static_cast<AddressFamily>(family);
}

int SocketAddress::GetPort() const
{
	int port = m_Buffer[2] << 8 & 0xFF00 | m_Buffer[3] & 0xFF;
	return port;
}

IPAddress SocketAddress::GetIPAddress() const
{
	__int64 address = static_cast<__int64>(
		(m_Buffer[4] & 0x000000FF) |
		(m_Buffer[5] << 8 & 0x0000FF00) |
		(m_Buffer[6] << 16 & 0x00FF0000) |
		(m_Buffer[7] << 24 & 0xFF000000)
		) & 0x00000000FFFFFFFF;
	return IPAddress(address);
}

IPEndPoint SocketAddress::GetIPEndPoint() const
{
	IPAddress address = GetIPAddress();
	int port = GetPort();

	return IPEndPoint(address, port);
}
