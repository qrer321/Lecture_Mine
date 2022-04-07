#include "PreCompile.h"
#include "GameServerSectionThread.h"

GameServerSectionThread::GameServerSectionThread()
{
	m_Sections.reserve(100);
}

void GameServerSectionThread::ThreadFunction()
{
	m_Timer.Reset();

	while (true)
	{
		if (0 != m_InsertSectionSize)
		{
			std::lock_guard lock(m_InsertSectionLock);
			for (const auto& insert_section : m_InsertSections)
			{
				insert_section->AccTimeReset();
				m_Sections.push_back(insert_section);
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
		}
		Sleep(1);
	}
}

void GameServerSectionThread::AddSection(const std::shared_ptr<GameServerSection>& section)
{
	++m_InsertSectionSize;
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
			section_element->IsDeath();	// -> ???
		}
	}

	++m_DeleteSectionSize;
}

void GameServerSectionThread::StartSectionThread()
{
	Start(&GameServerSectionThread::ThreadFunction, this);
}
