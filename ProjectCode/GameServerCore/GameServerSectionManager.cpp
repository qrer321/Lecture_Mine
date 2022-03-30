#include "PreCompile.h"
#include "GameServerSectionManager.h"

GameServerSectionManager* GameServerSectionManager::m_Inst = new GameServerSectionManager();

GameServerSectionManager::GameServerSectionManager()
	: m_NextSectionIndex(1)
{
}

uint32_t GameServerSectionManager::AddSection(GameServerSection* section)
{
	if (nullptr == section)
	{
		return 0;
	}

	if (0 != section->GetSectionNumber())
	{
		return section->GetSectionNumber();
	}

	{
		std::lock_guard lock(m_SectionLock);
		section->SetSectionNumber(m_NextSectionIndex++);
		
		m_AllSection.insert(std::unordered_map<uint32_t, std::shared_ptr<GameServerSection>>::value_type(m_NextSectionIndex, section));
	}

	return m_NextSectionIndex;
}

uint32_t GameServerSectionManager::RemoveSection(uint32_t section_number)
{
	std::lock_guard lock(m_SectionLock);

	m_AllSection.erase(section_number);
	return section_number;
}