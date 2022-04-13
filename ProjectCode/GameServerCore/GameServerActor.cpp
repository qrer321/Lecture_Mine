#include "PreCompile.h"
#include "GameServerActor.h"

GameServerActor::GameServerActor()
	: m_Section(nullptr)
	, m_ActorIndex(-1)
	, m_ThreadIndex(-1)
	, m_SectionIndex(-1)
	, m_ActorPos(FVector4::ZeroVector)
	, m_ActorDir(FVector4::ZeroVector)
{
}

std::shared_ptr<GameServerMessage> GameServerActor::PopMessage()
{
	if (true == IsEmptyMessage())
	{
		return nullptr;
	}

	std::shared_ptr<GameServerMessage> server_message = m_MessageQueue.front();
	m_MessageQueue.pop();

	return server_message;
}
