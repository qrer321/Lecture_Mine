#include "PreCompile.h"
#include "GameServerThread.h"
#include <Windows.h>

GameServerThread::GameServerThread() = default;

GameServerThread::~GameServerThread() = default;

GameServerThread::GameServerThread(GameServerThread&& other) noexcept
{

}

void GameServerThread::ThreadNameSetting(const std::string& name)
{
	std::wstring wName;
	wName.assign(name.begin(), name.end());
	SetThreadDescription(GetCurrentThread(), wName.c_str());
}

void GameServerThread::join()
{
	m_Thread.join();
}