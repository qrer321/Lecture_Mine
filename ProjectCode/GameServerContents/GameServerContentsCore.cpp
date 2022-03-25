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