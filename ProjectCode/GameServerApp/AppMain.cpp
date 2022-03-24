#include "PreCompile.h"
#include <GameServerCore/GameServerCore.h>
#include <GameServerContents/GameServerContentsCore.h>

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(25108);

	GameServerCore::CoreStart<GameServerContentsCore>();

	//DispatcherRegistration();

	//// 접속자를 받기 위한 비동기 소켓 초기화
	//PtrSTCPListener listener = std::make_shared<TCPListener>();
	//listener->Initialize(IPEndPoint(IPAddress::Parse("0.0.0.0"), 30001), [](const PtrSTCPSession& s)
	//	{
	//		s->SetCallBack([&](PtrSTCPSession session, const std::vector<unsigned char>& value)
	//			{
	//				MessageConverter converter = MessageConverter(value);
	//				if (false == converter.IsValid())
	//				{
	//					// converter가 유효하지 않다면
	//					// 클라이언트와 연결을 끊는다.
	//					GameServerDebug::AssertDebugMsg("Invalid Message");
	//					return;
	//				}

	//			// n초 동안 연락이 두절된 커넥션도 좀비 커넥션이라 보고 잘라내어야 한다.
	//			// -> 메시지 처리한 시간을 기록하여야 한다.

	//				MessageHandler<TCPSession> handler;
	//				if (false == g_dispatcher.GetHandler(converter.GetMessageType_UINT(), handler))
	//				{
	//					// 잘못 처리한 시간 기록


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