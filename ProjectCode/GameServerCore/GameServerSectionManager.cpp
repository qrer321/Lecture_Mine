#include "PreCompile.h"
#include "GameServerSectionManager.h"

GameServerSectionManager* GameServerSectionManager::m_Inst = new GameServerSectionManager();

GameServerSectionManager::~GameServerSectionManager()
{
	for (const auto& section_thread : m_SectionThread)
	{
		section_thread->m_SectionThreadQueue.Destroy();
	}
}

void GameServerSectionManager::Init(int thread_count, const std::string& thread_name)
{
	for (int i = 0; i < thread_count; ++i)
	{
		std::shared_ptr<GameServerSectionThread> new_section_thread = std::make_shared<GameServerSectionThread>();
		new_section_thread->StartSectionThread();
		new_section_thread->SetThreadIndex(i);
		new_section_thread->SetName(thread_name + std::to_string(i));
		m_SectionThread.push_back(new_section_thread);
	}
}

void GameServerSectionManager::CreateSection(int thread_index, uint64_t section_index, const std::shared_ptr<GameServerSection>& section)
{
	if (nullptr == section)
	{
		return;
	}
	
	{
		std::lock_guard lock(m_SectionLock);

		section->SetSectionIndex(section_index);
		section->SetThreadIndex(thread_index);
		m_AllSection.insert(std::unordered_map<uint64_t, std::shared_ptr<GameServerSection>>::value_type(section_index, section));
		m_SectionThread[thread_index]->AddSection(section);
	}

	return;
}

uint64_t GameServerSectionManager::RemoveSection(int thread_index, uint64_t section_index)
{
	std::lock_guard lock(m_SectionLock);

	m_AllSection.erase(section_index);
	return section_index;
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

void GameServerSectionManager::MessagePost(uint64_t thread_index, const std::function<void()>& callback)
{
	m_SectionThread[thread_index]->m_SectionThreadQueue.EnQueue(callback);
}

void GameServerSectionManager::ActorPost(uint64_t thread_index, uint64_t section_index, uint64_t actor_index, const std::shared_ptr<GameServerMessage>& message)
{
	if (thread_index >= m_SectionThread.size())
	{
		GameServerDebug::AssertDebugMsg("Message Sent To Section That Does Not Exist");
		return;
	}

	const std::shared_ptr<GameServerSectionThread>& section_thread = m_SectionThread[thread_index];
	section_thread->m_SectionThreadQueue.EnQueue(std::bind(&GameServerSectionThread::ActorPost, section_thread.get(), section_index, actor_index, message));
}
