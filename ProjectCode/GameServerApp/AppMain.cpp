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
	listener->Initialize(IPEndPoint(IPAddress::Parse("0.0.0.0"), 30001), [](PtrSTCPSession s)
		{
			s->SetCallBack([](const PtrSTCPSession& session, const std::vector<char>& value)
				{
					std::string strTest = &value[0];
					GameServerDebug::LogInfo(strTest);

					strTest = "to Server : " + strTest;
					std::vector<char> testVector = std::vector<char>(strTest.size() + 1);
					std::copy(strTest.begin(), strTest.end(), testVector.begin());
					testVector[strTest.size()] = 0;

					session->Send(testVector);
				},
				[](const PtrSTCPSession& session)
				{
					std::string logText = std::to_string(static_cast<int>(session->GetSocket()));
					GameServerDebug::LogInfo(logText + " 접속자가 종료했습니다");
				});

			std::string logText = std::to_string(static_cast<int>(s->GetSocket()));
			GameServerDebug::LogInfo(logText + " 접속자가 있습니다");
			{
				std::string strTest = "OSI7 계층중 1계층의 명사를 패킷으로 쏘시오!!";
				std::vector<char> testVector = std::vector<char>(strTest.size() + 1);
				std::copy(strTest.begin(), strTest.end(), testVector.begin());
				testVector[strTest.size()] = 0;

				for (const char i : testVector)
				{
					std::vector<char> TestP = std::vector<char>(2);
					TestP[0] = i;
					TestP[1] = 0;
					s->Send(TestP);
					Sleep(100);
				}
			}
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