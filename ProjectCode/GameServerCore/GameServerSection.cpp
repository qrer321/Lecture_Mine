#include "PreCompile.h"
#include "GameServerSection.h"

#include <GameServerNet/TCPSession.h>
#include <GameServerNet/UDPSession.h>

#include <utility>
#include "GameServerActor.h"
#include "GameServerCollision.h"

GameServerSection::GameServerSection()
	: m_SectionThread(nullptr)
{
	m_WaitActor.reserve(100);
}

void GameServerSection::InsertActor(uint64_t id, const std::shared_ptr<GameServerActor>& actor)
{
	if (-1 == id)
	{
		GameServerDebug::AssertDebugMsg("ID Cat Not Be -1");
		return;
	}

	std::lock_guard lock(m_WaitLock);
	actor->SetSectionIndex(GetSectionIndex());
	actor->SetThreadIndex(GetThreadIndex());
	m_WaitActor.push_back(actor);
	m_WaitActorCount = m_WaitActor.size();
}

GameServerCollision* GameServerSection::CreateCollision(int collision_type, GameServerActor* owner_actor)
{
	GameServerCollision* new_collision = new GameServerCollision;
	new_collision->SetGroupIndex(collision_type);
	new_collision->m_OwnerSection = this;
	new_collision->m_OwnerActor = owner_actor;

	m_CollisionList[collision_type].push_back(new_collision);

	return new_collision;
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

	for (const auto& actor : m_MoveActors)
	{
		m_AllActor.erase(actor.m_MoveActor->GetActorIndex());
		m_PlayableActor.remove(actor.m_MoveActor);
		actor.m_NextSection->MoveActor(actor.m_MoveActor);
	}
	m_MoveActors.clear();

	if (0 != m_WaitActorCount)
	{
		std::lock_guard lock(m_WaitLock);
		for (const auto& wait_actor : m_WaitActor)
		{
			if (nullptr != wait_actor->GetTCPSession())
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
			wait_actor->SectionMoveEnd();
		}

		m_WaitActor.clear();
		m_WaitActorCount = 0;
	}

	return true;
}

void GameServerSection::Release()
{
	{
		std::list<std::shared_ptr<GameServerActor>>::iterator start_iter = m_PlayableActor.begin();
		std::list<std::shared_ptr<GameServerActor>>::iterator end_iter = m_PlayableActor.end();

		while (start_iter != end_iter)
		{
			if (false == (*start_iter)->IsDeath())
			{
				++start_iter;
				continue;
			}

			(*start_iter)->DeathEvent();
			m_AllActor.erase((*start_iter)->GetActorIndex());
			start_iter = m_PlayableActor.erase(start_iter);
		}
	}

	{
		std::list<std::shared_ptr<GameServerActor>>::iterator start_iter = m_AIActor.begin();
		std::list<std::shared_ptr<GameServerActor>>::iterator end_iter = m_AIActor.end();

		while (start_iter != end_iter)
		{
			if (false == (*start_iter)->IsDeath())
			{
				++start_iter;
				continue;
			}

			(*start_iter)->DeathEvent();
			m_AllActor.erase((*start_iter)->GetActorIndex());
			start_iter = m_AIActor.erase(start_iter);
		}
	}
}

void GameServerSection::Broadcasting_TCP(const std::vector<unsigned char>& buffer, uint64_t ignore_actor /*= -1*/)
{
	for (const auto& actor : m_PlayableActor)
	{
		if (ignore_actor == actor->GetActorIndex())
		{
			continue;
		}

		actor->GetTCPSession()->Send(buffer);
	}
}

void GameServerSection::Broadcasting_UDP(const std::vector<unsigned char>& buffer, uint64_t ignore_actor)
{
	for (const auto& actor : m_PlayableActor)
	{
		if (ignore_actor == actor->GetActorIndex())
		{
			continue;
		}

		if (0 == actor->GetUDPEndPoint_Ref().GetPort())
		{
			continue;
		}

		actor->GetUDPSession()->Send(buffer, actor->GetUDPEndPoint_Ref());
	}
}

void GameServerSection::ActorPost(uint64_t actor_index, const std::shared_ptr<GameServerMessage>& message)
{
	const std::map<uint64_t, std::shared_ptr<GameServerActor>>::iterator find_iter = m_AllActor.find(actor_index);

	if (m_AllActor.end() == find_iter)
	{
		GameServerDebug::AssertDebugMsg("Message Sent To Invalid Actor");
		return;
	}

	if (nullptr == find_iter->second)
	{
		GameServerDebug::AssertDebugMsg("Message Sent To Invalid Actor");
		return;
	}

	if (true == find_iter->second->m_IsSectionMove)
	{
		return;
	}

	find_iter->second->m_MessageQueue.push(message);
}

void GameServerSection::ActorEndPointPost(uint64_t actor_index, const IPEndPoint& end_point, const std::shared_ptr<GameServerMessage>& message)
{
	const std::map<uint64_t, std::shared_ptr<GameServerActor>>::iterator find_iter = m_AllActor.find(actor_index);

	if (m_AllActor.end() == find_iter)
	{
		//GameServerDebug::AssertDebugMsg("Message Sent To Invalid Actor");
		return;
	}

	if (nullptr == find_iter->second)
	{
		GameServerDebug::AssertDebugMsg("Message Sent To Invalid Actor");
		return;
	}

	find_iter->second->m_UDPEndPoint = end_point;
	find_iter->second->m_MessageQueue.push(message);
}

void GameServerSection::MoveActor(const std::shared_ptr<GameServerActor>& actor)
{
	actor->SetSection(nullptr);
	InsertActor(actor->GetActorIndex(), actor);
}

void GameServerSection::MoveSection(const std::shared_ptr<GameServerActor>& move_actor, GameServerSection* dest_section)
{
	if (nullptr == move_actor || nullptr == dest_section)
	{
		return;
	}

	m_MoveActors.push_back({ move_actor, dest_section });
}
