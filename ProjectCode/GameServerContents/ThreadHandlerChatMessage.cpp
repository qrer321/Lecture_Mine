#include "PreCompile.h"
#include "ThreadHandlerChatMessage.h"

void ThreadHandlerChatMessage::Start()
{
	if (nullptr == m_Session)
	{
		GameServerDebug::LogError("Chat TCPSession Error");
		return;
	}

	/*GameServerSerializer serializer;
	m_Message->Serialize(serializer);

	auto* listener = m_Session->GetParent<TCPListener>();
	if (nullptr == listener)
	{
		GameServerDebug::AssertDebugMsg("Can't Broadcast Because TCPSession's Parent Is Not TCPListener");
		return;
	}

	listener->BroadCast(serializer.GetData(), m_Session);*/
}