#include "PreCompile.h"
#include "GameServerSection.h"
#include "GameServerActor.h"

GameServerSection::GameServerSection()
{
	m_WaitActor.reserve(100);
}

void GameServerSection::InsertActor(uint64_t id, const std::shared_ptr<GameServerActor>& actor)
{
	std::lock_guard lock(m_WaitLock);
	m_WaitActor.push_back(actor);
}

bool GameServerSection::Update()
{
	for (const auto& actor : m_UpdateActor)
	{
		if (true == actor->IsUpdate())
		{
			continue;
		}

		actor->Update();
	}

	{
		std::lock_guard lock(m_WaitLock);
		for (const auto& wait_actor : m_WaitActor)
		{
			m_UpdateActor.push_back(wait_actor);
			m_AllActor.insert(std::make_pair(wait_actor->GetID(), wait_actor));
		}

		m_WaitActor.clear();
	}

	return true;
}
