#include "PreCompile.h"
#include <iostream>
#include <conio.h>
#include <GameServerBase/GameServerDebug.h>
#include <GameServerBase/GameServerIocp.h>
#include <GameServerBase/GameServerQueue.h>
#include <GameServerBase/GameServerString.h>
#include <GameServerNet/TCPListener.h>
#include <GameServerNet/TCPSession.h>
#include <GameServerNet/ServerHelper.h>

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	ServerHelper::StartEngineStartUp();
	GameServerDebug::Initialize();

	// 접속자를 받기 위한 비동기 소켓 초기화
	PtrSTCPListener listener = std::make_shared<TCPListener>();
	listener->Initialize(IPEndPoint(IPAddress::Parse("0.0.0.0"), 30001), [](PtrSTCPSession s)
		{
			s->SetCallBack([](const PtrSTCPSession& session, const std::vector<char>& value)
				{
					std::string utf8 = &value[0];
					std::string ansi;
					GameServerString::UTF8ToAnsi(utf8, ansi);

					session->Send(value);
					GameServerDebug::LogInfo(ansi + "test");
				},
				[](const PtrSTCPSession& session)
				{
					std::string logText = std::to_string(static_cast<int>(session->GetSocket()));
					GameServerDebug::LogInfo(logText + " 접속자가 종료했습니다");
				});

			std::string logText = std::to_string(static_cast<int>(s->GetSocket()));
			GameServerDebug::LogInfo(logText + " 접속자가 있습니다");
		});

	GameServerQueue networkQueue;
	networkQueue.Initialize(GameServerQueue::WORK_TYPE::Default, 8, "Network");
	listener->BindQueue(networkQueue);
	listener->StartAccept(10);

	GameServerDebug::LogInfo("서버 시작");

	_getch();

	networkQueue.Destroy();
	GameServerDebug::Destroy();
	
	return 0;
}