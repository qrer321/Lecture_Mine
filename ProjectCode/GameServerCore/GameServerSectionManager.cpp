#include "PreCompile.h"
#include "GameServerSectionManager.h"

GameServerSectionManager* GameServerSectionManager::m_Inst = new GameServerSectionManager();

void GameServerSectionManager::Init(int thread_count)
{
	for (int i = 0; i < thread_count; ++i)
	{
		std::shared_ptr<GameServerSectionThread> new_section_thread = std::make_shared<GameServerSectionThread>();
		new_section_thread->StartSectionThread();
		m_SectionThread.push_back(new_section_thread);
	}
}

void GameServerSectionManager::CreateSection(int thread_key, uint64_t section_key, std::shared_ptr<GameServerSection> section)
{
	if (nullptr == section)
	{
		return;
	}
	
	{
		std::lock_guard lock(m_SectionLock);

		section->SetSectionKey(section_key);
		m_AllSection.insert(std::unordered_map<uint64_t, std::shared_ptr<GameServerSection>>::value_type(section_key, section));
		m_SectionThread[thread_key]->AddSection(section);
	}

	return;
}

uint64_t GameServerSectionManager::RemoveSection(int thread_key, uint64_t section_key)
{
	std::lock_guard lock(m_SectionLock);

	m_AllSection.erase(section_key);
	return section_key;
}

std::shared_ptr<GameServerSection> GameServerSectionManager::FindSection(uint64_t section_key)
{
	std::lock_guard lock(m_SectionLock);
	const auto find_iter = m_AllSection.find(section_key);
	if (find_iter == m_AllSection.end())
	{
		return nullptr;
	}

	return find_iter->second;
}