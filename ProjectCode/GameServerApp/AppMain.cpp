#include "PreCompile.h"
#include <iostream>
#include <conio.h>
#include <GameServerBase/GameServerDebug.h>
#include <GameServerBase/GameServerIocp.h>
#include <GameServerBase/GameServerQueue.h>
#include <GameServerNet/TCPListener.h>
#include <GameServerNet/TCPSession.h>
#include <GameServerNet/ServerHelper.h>

int main()
{
	ServerHelper::StartEngineStartUp();
	GameServerDebug::Initialize();

	// 접속자를 받기 위한 비동기 소켓 초기화
	PtrSTCPListener listener = std::make_shared<TCPListener>();
	listener->Initialize(IPEndPoint(IPAddress::Parse("127.0.0.1"), 30001), [](PtrSTCPSession s)
		{
			GameServerDebug::LogInfo("접속자가 있습니다");
		});

	GameServerQueue networkQueue;
	networkQueue.Initialize(GameServerQueue::WORK_TYPE::Default, 10, "Network");

	listener->BindQueue(networkQueue);

	return 0;
}