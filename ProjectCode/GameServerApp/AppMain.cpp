#include "PreCompile.h"
#include <iostream>
#include <conio.h>
#include <GameServerBase/GameServerDebug.h>
#include <GameServerBase/GameServerIocp.h>
#include "TestClass.h"

int main()
{
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