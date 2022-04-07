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
	m_WaitActorCount = m_WaitActor.size();
}

bool GameServerSection::Update(float delta_time)
{
	UserUpdate();

	for (const auto& actor : m_PlayableActor)
	{
		if (false == actor->IsUpdate())
		{
			continue;
		}

		actor->AccTimeUpdate(delta_time);
		actor->Update(delta_time);
	}

	for (const auto& actor : m_AIActor)
	{
		if (false == actor->IsUpdate())
		{
			continue;
		}

		actor->AccTimeUpdate(delta_time);
		actor->Update(delta_time);
	}

	if (0 != m_WaitActorCount)
	{
		std::lock_guard lock(m_WaitLock);
		for (const auto& wait_actor : m_WaitActor)
		{
			if (nullptr != wait_actor->GetSession())
			{
				m_PlayableActor.push_back(wait_actor);
			}

			wait_actor->SetSection(this);
			wait_actor->AccTimeReset();

			if (false == wait_actor->InsertSection())
			{
				// Section에 넣어주는 것이 실패할 경우
				continue;
			}

			m_AllActor.insert(std::make_pair(wait_actor->GetID(), wait_actor));
		}

		m_WaitActor.clear();
		m_WaitActorCount = 0;
	}

	return true;
}

void GameServerSection::Broadcasting()
{
	for (const auto& actor : m_PlayableActor)
	{
		if (nullptr == actor->GetSession())
		{
			// Player Actor가 TCPSession을 들고있지 않는다
			// -> 잘못된 Player Actor
			continue;
		}
	}
}
