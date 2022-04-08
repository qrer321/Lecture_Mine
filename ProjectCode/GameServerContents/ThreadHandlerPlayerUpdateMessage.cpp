#include "PreCompile.h"
#include "ThreadHandlerPlayerUpdateMessage.h"

void ThreadHandlerPlayerUpdateMessage::Start()
{
	if (nullptr == m_TCPSession)
	{
		GameServerDebug::LogError("Player Update TCPSession Error");
		return;
	}
}
