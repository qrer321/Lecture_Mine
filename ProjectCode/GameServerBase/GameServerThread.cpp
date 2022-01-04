#include "PreCompile.h"
#include "GameServerThread.h"
#include <Windows.h>

GameServerThread::GameServerThread() = default;

GameServerThread::~GameServerThread() = default;

GameServerThread::GameServerThread(GameServerThread&& other) noexcept
{

}

void GameServerThread::ThreadNameSetting(const wchar_t* name)
{
	SetThreadDescription(GetCurrentThread(), name);
}

void GameServerThread::join()
{
	m_Thread.join();
}