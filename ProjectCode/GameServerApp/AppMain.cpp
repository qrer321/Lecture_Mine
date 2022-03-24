#include "PreCompile.h"
#include <GameServerCore/GameServerCore.h>
#include <GameServerContents/GameServerContentsCore.h>

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(25108);

	GameServerCore::CoreStart<GameServerContentsCore>();

	//DispatcherRegistration();

	//// �����ڸ� �ޱ� ���� �񵿱� ���� �ʱ�ȭ
	//PtrSTCPListener listener = std::make_shared<TCPListener>();
	//listener->Initialize(IPEndPoint(IPAddress::Parse("0.0.0.0"), 30001), [](const PtrSTCPSession& s)
	//	{
	//		s->SetCallBack([&](PtrSTCPSession session, const std::vector<unsigned char>& value)
	//			{
	//				MessageConverter converter = MessageConverter(value);
	//				if (false == converter.IsValid())
	//				{
	//					// converter�� ��ȿ���� �ʴٸ�
	//					// Ŭ���̾�Ʈ�� ������ ���´�.
	//					GameServerDebug::AssertDebugMsg("Invalid Message");
	//					return;
	//				}

	//			// n�� ���� ������ ������ Ŀ�ؼǵ� ���� Ŀ�ؼ��̶� ���� �߶󳻾�� �Ѵ�.
	//			// -> �޽��� ó���� �ð��� ����Ͽ��� �Ѵ�.

	//				MessageHandler<TCPSession> handler;
	//				if (false == g_dispatcher.GetHandler(converter.GetMessageType_UINT(), handler))
	//				{
	//					// �߸� ó���� �ð� ���


	//					GameServerDebug::AssertDebugMsg("Invalid Message");
	//					return;
	//				}

	//				handler(std::move(session), std::move(converter.GetServerMessage()));
	//			},
	//			[](const PtrSTCPSession& session)
	//			{
	//				std::string logText = std::to_string(static_cast<int>(session->GetSocket()));
	//				GameServerDebug::LogInfo(logText + " Connection has Ended");
	//			});

	//		std::string logText = std::to_string(static_cast<int>(s->GetSocket()));
	//		GameServerDebug::LogInfo(logText + " Connected");
	//	});

	//listener->BindQueue(NetQueue::GetQueue());
	//listener->StartAccept(10);

	//GameServerDebug::LogInfo("Server Start");

	//_getch();

	return 0;
}