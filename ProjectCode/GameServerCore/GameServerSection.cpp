#include "PreCompile.h"
#include "GameServerSection.h"
#include "GameServerActor.h"
#include "GameServerNet/TCPSession.h"

GameServerSection::GameServerSection()
	: m_SectionThread(nullptr)
{
	m_WaitActor.reserve(100);
}

void GameServerSection::InsertActor(const std::shared_ptr<GameServerActor>& actor)
{
	std::lock_guard lock(m_WaitLock);
	actor->SetSectionIndex(GetSectionIndex());
	actor->SetThreadIndex(GetThreadIndex());
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
			else
			{
				m_AIActor.push_back(wait_actor);
			}

			wait_actor->SetSection(this);
			wait_actor->AccTimeReset();

			if (false == wait_actor->InsertSection())
			{
				// Section에 넣어주는 것이 실패할 경우
				continue;
			}

			m_AllActor.insert(std::make_pair(wait_actor->GetActorIndex(), wait_actor));
		}

		m_WaitActor.clear();
		m_WaitActorCount = 0;
	}

	return true;
}

void GameServerSection::Broadcasting(const std::vector<unsigned char>& buffer, uint64_t ignore_actor /*= -1*/)
{
	for (const auto& actor : m_PlayableActor)
	{
		if (ignore_actor == actor->GetActorIndex())
		{
			continue;
		}

		actor->GetSession()->Send(buffer);
	}
}
