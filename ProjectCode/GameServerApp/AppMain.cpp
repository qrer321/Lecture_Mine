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
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	ServerHelper::StartEngineStartUp();
	GameServerDebug::Initialize();

	// 접속자를 받기 위한 비동기 소켓 초기화
	PtrSTCPListener listener = std::make_shared<TCPListener>();
	listener->Initialize(IPEndPoint(IPAddress::Parse("127.0.0.1"), 30001), [](PtrSTCPSession s)
		{
			// 내부적인 처리가 끝나고
			// 어떤 게임을 만들어야 할지는
			// 이 곳에서 처리한다
			// (어플리케이션 설정)

			// 서버로 나뉘는 게임인지, 채널로 나뉘는 게임인지 결정

			// 접속자가 존재하고 해당 접속자와 세션을 연결하면
			// 최종적으로 처리할 함수가 이 곳

			// 세션이 연결된 접속자는 IP / PW를 전달하고
			// DB를 통해 서버의 인정을 받아야 한다
			s->SetCallBack([](PtrSTCPSession callback, const std::vector<char>& value)
				{
					std::string strTest = &value[0];
					GameServerDebug::LogInfo(strTest);
				}, nullptr);
			GameServerDebug::LogInfo("접속자가 있습니다");
		});

	GameServerQueue networkQueue;
	networkQueue.Initialize(GameServerQueue::WORK_TYPE::Default, 1, "Network");
	listener->BindQueue(networkQueue);

	// 접속자 받기 시작
	listener->StartAccept(1);

	_getch();

	networkQueue.Destroy();
	GameServerDebug::Destroy();
	
	return 0;
}