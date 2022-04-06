#include "PreCompile.h"
#include "GameServerSectionThread.h"

GameServerSectionThread::GameServerSectionThread()
	: m_LastIndex(0)
	, m_SectionSize(0)
{
	m_Sections.reserve(100);
}

void GameServerSectionThread::ThreadFunction(GameServerSectionThread* section_thread)
{
	if (nullptr == section_thread)
	{
		GameServerDebug::LogErrorAssert("Section Thread Is Nullptr");
		return;
	}

	std::vector<GameServerSection*> delete_sections;
	while (true)
	{
		const size_t section_size = section_thread->m_SectionSize;
		for (size_t i = 0; i < section_size; ++i)
		{
			if (nullptr == section_thread->m_Sections[i])
			{
				continue;
			}

			section_thread->m_Sections[i]->Update();
		}
		Sleep(1);
	}
}

void GameServerSectionThread::AddSection(const std::shared_ptr<GameServerSection>& section)
{
	++m_SectionSize;
	m_Sections.push_back(section);
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
			section_element = nullptr;
		}
	}
}

void GameServerSectionThread::StartSectionThread()
{
	Start(ThreadFunction, this);
}
