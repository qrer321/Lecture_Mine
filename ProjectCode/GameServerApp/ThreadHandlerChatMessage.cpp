#include "PreCompile.h"
#include "ThreadHandlerChatMessage.h"
#include <GameServerBase/GameServerDebug.h>

#include "GameServerNet/TCPListener.h"

ThreadHandlerChatMessage::ThreadHandlerChatMessage(std::shared_ptr<TCPSession> tcp_session, std::shared_ptr<ChatMessage> message)
	: m_TCPSession(std::move(tcp_session))
	, m_Message(std::move(message))
{
}

void ThreadHandlerChatMessage::Start()
{
	if (nullptr == m_TCPSession)
	{
		GameServerDebug::LogError("Chat TCPSession Error");
	}

	GameServerSerializer serializer;
	m_Message->Serialize(serializer);

	auto* listener = m_TCPSession->GetParent<TCPListener>();
	if (nullptr == listener)
	{
		GameServerDebug::AssertDebugMsg("Can't Broadcast Because TCPSession's Parent Is Not TCPListener");
		return;
	}

	listener->BroadCast(serializer.GetData(), m_TCPSession);
}