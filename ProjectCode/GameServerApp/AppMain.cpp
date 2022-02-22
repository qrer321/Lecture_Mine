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
#include <GameServerMessage/Messages.h>
#include <GameServerMessage/MessageConverter.h>
#include <GameServerMessage/Dispatcher.h>
#include "DBQueue.h"
#include "NetQueue.h"

#include "ServerDispatcher.h"

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	ServerHelper::ServerStartup();
	GameServerDebug::Initialize();

	DBQueue::Init();
	NetQueue::Init();

	DispatcherRegistration();

	// �����ڸ� �ޱ� ���� �񵿱� ���� �ʱ�ȭ
	PtrSTCPListener listener = std::make_shared<TCPListener>();
	listener->Initialize(IPEndPoint(IPAddress::Parse("0.0.0.0"), 30001), [](const PtrSTCPSession& s)
		{
			s->SetCallBack([&](PtrSTCPSession session, const std::vector<unsigned char>& value)
				{
					MessageConverter converter = MessageConverter(value);
					if (false == converter.IsValid())
					{
						// converter�� ��ȿ���� �ʴٸ�
						// Ŭ���̾�Ʈ�� ������ ���´�.
						GameServerDebug::AssertDebugMsg("Invalid Message");
						return;
					}

				// n�� ���� ������ ������ Ŀ�ؼǵ� ���� Ŀ�ؼ��̶� ���� �߶󳻾�� �Ѵ�.
				// -> �޽��� ó���� �ð��� ����Ͽ��� �Ѵ�.

					MessageHandler<TCPSession> handler;
					if (false == g_dispatcher.GetHandler(converter.GetMessageType_UINT(), handler))
					{
						// �߸� ó���� �ð� ���


						GameServerDebug::AssertDebugMsg("Invalid Message");
						return;
					}

					handler(std::move(session), std::move(converter.GetServerMessage()));
				},
				[](const PtrSTCPSession& session)
				{
					std::string logText = std::to_string(static_cast<int>(session->GetSocket()));
					GameServerDebug::LogInfo(logText + " Connection has Ended");
				});

			std::string logText = std::to_string(static_cast<int>(s->GetSocket()));
			GameServerDebug::LogInfo(logText + " Connected");
		});

	listener->BindQueue(NetQueue::GetQueue());
	listener->StartAccept(10);

	GameServerDebug::LogInfo("Server Start");

	_getch();

	DBQueue::Destroy();
	NetQueue::Destroy();

	GameServerDebug::Destroy();
	
	return 0;
}