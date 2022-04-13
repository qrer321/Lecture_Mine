#include "PreCompile.h"
#include "GameServerThread.h"
#include <Windows.h>

thread_local std::string			GameServerThread::m_ThreadName;
thread_local unsigned int			GameServerThread::m_Order;
thread_local const std::type_info*	GameServerThread::m_LocalDataType = nullptr;
thread_local std::vector<char>		GameServerThread::m_Data;
thread_local GameServerTime			GameServerThread::m_Timer;

void GameServerThread::SetThreadName(const std::string& name)
{
	m_ThreadName = name;

	std::wstring thread_name;
	thread_name.assign(name.begin(), name.end());
	SetThreadDescription(GetCurrentThread(), thread_name.c_str());
}

void GameServerThread::SetThreadOrder(unsigned int order)
{
	m_Order = order;
}

void GameServerThread::join()
{
	m_Thread.join();
}
