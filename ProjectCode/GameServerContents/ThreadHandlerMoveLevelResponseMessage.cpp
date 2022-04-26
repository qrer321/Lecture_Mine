#include "PreCompile.h"
#include "ThreadHandlerMoveLevelResponseMessage.h"
#include <GameServerCore/GameServerSectionManager.h>

void ThreadHandlerMoveLevelResponseMessage::Start()
{
	if (nullptr == m_Session)
	{
		GameServerDebug::LogError("Move Level Response TCPSession Error");
		return;
	}

	ActorWork(m_Message->m_ThreadIndex, m_Message->m_SectionIndex, m_Message->m_ActorIndex, m_Message);
}