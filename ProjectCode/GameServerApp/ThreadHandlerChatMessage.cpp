#include "PreCompile.h"
#include "ThreadHandlerChatMessage.h"
#include <GameServerBase/GameServerDebug.h>

ThreadHandlerChatMessage::ThreadHandlerChatMessage(std::shared_ptr<TCPSession> tcp_session, std::shared_ptr<ChatMessage> message)
	: m_TCPSession(std::move(tcp_session))
	, m_Message(std::move(message))
{
}

void ThreadHandlerChatMessage::Start()
{
	if (nullptr == m_TCPSession)
	{
		GameServerDebug::LogError("Login TCPSession Error");
	}
}