#pragma once
#include <GameServerBase/GameServerQueue.h>
#include <GameServerBase/GameServerThread.h>
#include "GameServerSection.h"

class GameServerMessage;
class GameServerSectionThread : GameServerThread
{
	friend class GameServerSectionManager;

private: // Member Var
	GameServerQueue											m_SectionThreadQueue{};
	std::shared_ptr<std::thread>							m_Thread;
	uint64_t												m_ThreadIndex{};

	std::map<uint64_t, std::shared_ptr<GameServerSection>>	m_KeySections;
	std::vector<std::shared_ptr<GameServerSection>>			m_Sections;

	std::vector<std::shared_ptr<GameServerSection>>			m_InsertSections;
	std::atomic<size_t>										m_InsertSectionSize;
	std::mutex												m_InsertSectionLock;

	std::atomic<size_t>										m_DeleteSectionSize;

public: // Default
	GameServerSectionThread();
	~GameServerSectionThread() override = default;

	GameServerSectionThread(const GameServerSectionThread& other) = delete;
	GameServerSectionThread(GameServerSectionThread&& other) noexcept = delete;

	GameServerSectionThread& operator=(const GameServerSectionThread& other) = delete;
	GameServerSectionThread& operator=(GameServerSectionThread&& other) = delete;

private:
	void ThreadFunction();
	void SetThreadIndex(uint64_t thread_index) { m_ThreadIndex = thread_index; }

	void ActorPost(uint64_t section_index, uint64_t actor_index, const std::shared_ptr<GameServerMessage>& message);

public: // Member Function
	uint64_t GetThreadIndex() const { return m_ThreadIndex; }

	void AddSection(const std::shared_ptr<GameServerSection>& section);
	void RemoveSection(const std::shared_ptr<GameServerSection>& section);

	void StartSectionThread();
};

