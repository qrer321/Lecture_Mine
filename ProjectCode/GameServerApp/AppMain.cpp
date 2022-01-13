#include "PreCompile.h"
#include <iostream>
#include <conio.h>
#include <GameServerBase/GameServerDebug.h>
#include <GameServerBase/GameServerIocp.h>
#include <GameServerBase/GameServerQueue.h>
#include "TestClass.h"

int main()
{
	GameServerQueue newQueue = GameServerQueue(GameServerQueue::WORK_TYPE::Default, 1, "Recv");

	return 0;
}