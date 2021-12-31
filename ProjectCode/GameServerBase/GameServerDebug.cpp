#include "PreCompile.h"
#include "GameServerDebug.h"
#include <Windows.h>

GameServerDebug::GameServerDebug() 
{

}

GameServerDebug::~GameServerDebug() 
{

}

GameServerDebug::GameServerDebug(GameServerDebug&& _Other) noexcept
{

}

void GameServerDebug::AssertDebug()
{
	assert(false);
}

void GameServerDebug::AssertDebugMsg(const std::string& _Msg)
{
	MessageBoxA(nullptr, _Msg.c_str(), "∞Ê∞Ì√¢", MB_OK);
	AssertDebug();
}

