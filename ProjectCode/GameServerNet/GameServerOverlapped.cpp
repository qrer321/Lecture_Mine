#include "PreCompile.h"
#include "GameServerOverlapped.h"

////////////////// GameServerOverlapped //////////////////
GameServerOverlapped::GameServerOverlapped()
	: m_Overlapped()
{
	ResetOverlapped();
}

LPWSAOVERLAPPED GameServerOverlapped::GetOverlapped()
{
	return &m_Overlapped;
}

void GameServerOverlapped::ResetOverlapped()
{
	memset(&m_Overlapped, 0x00, sizeof(WSAOVERLAPPED));
}

////////////////// AcceptExOverlapped //////////////////
AcceptExOverlapped::AcceptExOverlapped(const PtrSTCPSession& tcpSession)
	: m_Buffer{}
	, m_TCPSession(nullptr)
{
}

AcceptExOverlapped::~AcceptExOverlapped()
= default;

void AcceptExOverlapped::Execute(BOOL result, DWORD byteSize)
{
}