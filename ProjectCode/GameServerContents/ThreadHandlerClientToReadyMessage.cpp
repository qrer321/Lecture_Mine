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

	GameServerSectionManager::GetInst()->MessagePost(m_Message->m_ThreadIndex, std::bind(&ThreadHandlerClientToReadyMessage::SectionWork, this));
}

void ThreadHandlerClientToReadyMessage::SectionWork()
{
	int a = 0;
}
