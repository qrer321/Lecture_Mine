#include "PreCompile.h"
#include "SocketBase.h"

SocketBase::SocketBase()
	: m_Socket(NULL)
{
}

SocketBase::~SocketBase()
= default;

SocketBase::SocketBase(SocketBase&& other) noexcept
	: m_Socket(other.m_Socket)
{

}

void SocketBase::Close()
{
	if (NULL != m_Socket)
	{
		closesocket(m_Socket);
		m_Socket = NULL;
	}
}

