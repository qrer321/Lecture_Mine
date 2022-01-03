#include "PreCompile.h"
#include "GameServerThread.h"
#include <Windows.h>

GameServerThread::GameServerThread() 
{

}

GameServerThread::~GameServerThread() 
{

}

GameServerThread::GameServerThread(GameServerThread&& _Other) noexcept
{

}

void GameServerThread::ThreadNameSetting(const wchar_t* name)
{
	SetThreadDescription(GetCurrentThread(), name);
}

