#include "PreCompile.h"
#include "GameServerSectionThread.h"

void GameServerSectionThread::ThreadFunction(GameServerSectionThread* thread)
{
	if (nullptr == thread)
	{
		GameServerDebug::LogErrorAssert("Section Thread Is Nullptr");
		return;
	}

	std::vector<GameServerSection*> delete_sections;
	while (true)
	{
		for (GameServerSection* section : thread->m_Sections)
		{
			if (false == section->Update())
			{
				delete_sections.push_back(section);
			}
		}

		for (GameServerSection* section : delete_sections)
		{
			thread->RemoveSection(section);
		}

		delete_sections.clear();
	}
}

void GameServerSectionThread::AddSection(GameServerSection* section)
{
}

void GameServerSectionThread::RemoveSection(GameServerSection* section)
{
	if (nullptr == section)
	{
		GameServerDebug::LogErrorAssert("Section Is Nullptr");
		return;
	}

	auto section_iter = m_Sections.begin();
	for (; section_iter != m_Sections.end(); ++section_iter)
	{
		if ((*section_iter)->GetSectionNumber() == section->GetSectionNumber())
		{
			delete (*section_iter);
			section_iter = m_Sections.erase(section_iter);
			return;
		}
	}
}