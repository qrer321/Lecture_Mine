#include "PreCompile.h"
#include <iostream>
#include <conio.h>
#include <GameServerBase/GameServerDebug.h>
#include <GameServerBase/GameServerIocp.h>
#include "TestClass.h"

GameServerIocp g_newIocp0;
GameServerIocp g_newIocp1;

void Func0(const std::shared_ptr<GameServerIocpWorker>& worker)
{
	while (true)
	{
		worker->Wait();

		char* Log = worker->ConvertKey<char*>();
		std::cout << Log << std::endl;
	}
}

std::atomic<int> g_valueA = 0;

void Func1(const std::shared_ptr<GameServerIocpWorker>& worker)
{
	while (true)
	{
		switch (worker->Wait())
		{
		case IocpWaitReturnType::RETURN_TIMEOUT:
			{
			std::string name = "Test";
			name += std::to_string(++g_valueA);
			g_newIocp0.Post(-1, reinterpret_cast<ULONG_PTR>(name.c_str()));

			int a = 0;

			break;
			}
		case IocpWaitReturnType::RETURN_POST:
			std::cout << "IOCP1" << " " << "Thread" << worker->GetIndex() << std::endl;
			break;
		default:
			break;
		}

		worker->Wait();
	}
}

int main()
{
	g_newIocp0.Initialize(Func0, 1, INFINITE);
	g_newIocp1.Initialize(Func1, 10, 100);

	while (true)
	{
		int value = _getch();
	}

	return 0;
}