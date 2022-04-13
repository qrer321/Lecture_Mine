#include "PreCompile.h"
#include "ThreadHandlerPlayerUpdateMessage.h"

void ThreadHandlerPlayerUpdateMessage::Start()
{
	if (nullptr == m_TCPSession)
	{
		GameServerDebug::LogError("Player Update TCPSession Error");
		return;
	}

	ActorWork(m_Message->m_Datum.m_ThreadIndex, m_Message->m_Datum.m_SectionIndex, m_Message->m_Datum.m_ActorIndex, m_Message);
}
