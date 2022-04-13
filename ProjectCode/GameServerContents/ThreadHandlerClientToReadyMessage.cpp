#include "PreCompile.h"
#include "ThreadHandlerClientToReadyMessage.h"
#include <GameServerCore/GameServerSectionManager.h>

void ThreadHandlerClientToReadyMessage::Start()
{
	if (nullptr == m_TCPSession)
	{
		GameServerDebug::LogError("Client To Ready TCPSession Error");
		return;
	}

	ActorWork(m_Message->m_ThreadIndex, m_Message->m_SectionIndex, m_Message->m_ActorIndex, m_Message);
}