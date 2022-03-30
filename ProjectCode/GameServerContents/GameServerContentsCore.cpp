#include "PreCompile.h"
#include "GameServerContentsCore.h"
#include <GameServerNet/TCPSession.h>
#include <GameServerMessage/MessageConverter.h>
#include "ServerDispatcher.h"

void GameServerContentsCore::UserStart()
{
	DispatcherRegistration();

	SetAcceptCallback(AcceptEvent);
}

void GameServerContentsCore::AcceptEvent(const std::shared_ptr<TCPSession>& session)
{
	session->SetRecvCallBack(RecvEvent);
	session->SetCloseCallBack(CloseEvent);

	const std::string logText = std::to_string(static_cast<int>(session->GetSocket()));
	GameServerDebug::LogInfo(logText + " Socket Connected");
}

void GameServerContentsCore::RecvEvent(std::shared_ptr<TCPSession> session, const std::vector<unsigned char>& value)
{
	MessageConverter converter = MessageConverter(value);
	if (false == converter.IsValid())
	{
		// converter가 유효하지 않다면
		// 클라이언트와 연결을 끊는다.
		GameServerDebug::AssertDebugMsg("Invalid Message");
		return;
	}

	// n초 동안 연락이 두절된 커넥션도 좀비 커넥션이라 보고 잘라내어야 한다.
	// -> 메시지 처리한 시간을 기록하여야 한다.

	MessageHandler<TCPSession> handler;
	if (false == g_dispatcher.GetHandler(converter.GetMessageType(), handler))
	{
		GameServerDebug::AssertDebugMsg("Invalid Handler");
		return;
	}

	handler(std::move(session), std::move(converter.GetServerMessage()));
}

void GameServerContentsCore::CloseEvent(std::shared_ptr<TCPSession> session)
{
	const std::string logText = std::to_string(static_cast<int>(session->GetSocket()));
	GameServerDebug::LogInfo(logText + " Connection has Ended");
}