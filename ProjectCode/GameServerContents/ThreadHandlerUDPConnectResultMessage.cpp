#include "PreCompile.h"
#include "ThreadHandlerUDPConnectResultMessage.h"

void ThreadHandlerUDPConnectResultMessage::Start()
{
	if (nullptr == m_Session)
	{
		GameServerDebug::AssertDebugMsg("Connect UDPSession Is Nullptr");
		return;
	}

	ActorWork(m_Message->m_ThreadIndex, m_Message->m_SectionIndex, m_Message->m_ActorIndex, m_Message);
}
