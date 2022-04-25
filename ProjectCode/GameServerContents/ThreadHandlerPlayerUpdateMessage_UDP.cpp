#include "PreCompile.h"
#include "ThreadHandlerPlayerUpdateMessage_UDP.h"

void ThreadHandlerPlayerUpdateMessage_UDP::Start()
{
	if (nullptr == m_Session)
	{
		GameServerDebug::LogError("Player Update UDPSession Error");
		return;
	}

	ActorEndPointWork(m_Message->m_Datum.m_ThreadIndex, m_Message->m_Datum.m_SectionIndex, m_Message->m_Datum.m_ActorIndex, m_EndPoint, m_Message);
}
