#include "PreCompile.h"
#include <iostream>
#include <conio.h>
#include <GameServerBase/GameServerDebug.h>
#include <GameServerBase/GameServerIocp.h>
#include <GameServerBase/GameServerQueue.h>
#include "TestClass.h"

void Func()
{
	std::cout << "Test" << std::endl;
}

int main()
{
	GameServerQueue newQueue = GameServerQueue(GameServerQueue::WORK_TYPE::Default, 1, "Recv");
	while (true)
	{
		int input = _getch();
		if ('a' == input || 'A' == input)
			newQueue.EnQueue(Func);
		else if ('q' == input || 'Q' == input)
		{
			newQueue.Destroy();
			break;
		}
	}

	return 0;

	GameServerDebug::Initialize();

	while (true)
	{
		GameServerDebug::LogError("test");

		int value = _getch();

		if (value == 'q')
			break;
	}

	GameServerDebug::Destroy();

	return 0;
}