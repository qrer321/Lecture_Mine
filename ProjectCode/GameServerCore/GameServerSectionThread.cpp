#include "PreCompile.h"
#include "GameServerSectionThread.h"
#include "GameServerSection.h"

GameServerSectionThread::GameServerSectionThread()
{
	m_Sections.reserve(100);
}

void GameServerSectionThread::ThreadFunction()
{
	SetThreadName(GetNameCopy());

	m_SectionThreadQueue.Initialize(1);
	m_SectionThreadQueue.SetExecuteType(GameServerQueue::WORK_TYPE::Default);
	m_Timer.Reset();

	GameServerQueue::QUEUE_RETURN return_result = GameServerQueue::QUEUE_RETURN::OK;
	while (GameServerQueue::QUEUE_RETURN::DESTROY != return_result)
	{
		return_result = GameServerQueue::QUEUE_RETURN::OK;
		while (GameServerQueue::QUEUE_RETURN::OK == return_result)
		{
			return_result = m_SectionThreadQueue.Execute(1);
		}

		if (GameServerQueue::QUEUE_RETURN::DESTROY == return_result)
		{
			return;
		}

		if (0 != m_InsertSectionSize)
		{
			std::lock_guard lock(m_InsertSectionLock);
			for (const auto& insert_section : m_InsertSections)
			{
				insert_section->AccTimeReset();
				m_Sections.push_back(insert_section);
				m_KeySections.insert(std::make_pair(insert_section->GetSectionIndex(), insert_section));
			}

			m_InsertSections.clear();
			m_InsertSectionSize = 0;
		}

		const float delta_time = m_Timer.Update();
		for (const auto& section : m_Sections)
		{
			if (false == section->IsUpdate())
			{
				continue;
			}

			section->AccTimeUpdate(delta_time);
			section->Update(delta_time);
			section->Release();
		}
		Sleep(1);
	}
}

void GameServerSectionThread::ActorPost(uint64_t section_index, uint64_t actor_index, const std::shared_ptr<GameServerMessage>& message)
{
	if (m_KeySections.end() == m_KeySections.find(section_index))
	{
		GameServerDebug::AssertDebugMsg("Message Sent To Section That Does Not Exist");
		return;
	}

	m_KeySections[section_index]->ActorPost(actor_index, message);
}

void GameServerSectionThread::AddSection(const std::shared_ptr<GameServerSection>& section)
{
	++m_InsertSectionSize;
	section->SetSectionThread(this);
	m_InsertSections.push_back(section);
}

void GameServerSectionThread::RemoveSection(const std::shared_ptr<GameServerSection>& section)
{
	if (nullptr == section)
	{
		GameServerDebug::LogErrorAssert("Section Is Nullptr");
		return;
	}

	for (auto& section_element : m_Sections)
	{
		if (section_element == section)
		{
			//section_element = nullptr;
			//section_element->IsDeath();	// -> ???
		}
	}

	++m_DeleteSectionSize;
}

void GameServerSectionThread::StartSectionThread()
{
	Start(&GameServerSectionThread::ThreadFunction, this);
}
