#pragma once
#include <GameServerBase/GameServerQueue.h>
#include <GameServerBase/GameServerThread.h>
#include "GameServerSection.h"

class GameServerSectionThread : GameServerThread
{
private: // Member Var
	GameServerQueue*								m_SectionTaskQueue{};
	std::shared_ptr<std::thread>					m_Thread;

	std::vector<std::shared_ptr<GameServerSection>> m_Sections;

	std::vector<std::shared_ptr<GameServerSection>> m_InsertSections;
	std::atomic<size_t>								m_InsertSectionSize;
	std::mutex										m_InsertSectionLock;

	std::atomic<size_t>								m_DeleteSectionSize;

public: // Default
	GameServerSectionThread();
	~GameServerSectionThread() override = default;

	GameServerSectionThread(const GameServerSectionThread& other) = delete;
	GameServerSectionThread(GameServerSectionThread&& other) noexcept = delete;

	GameServerSectionThread& operator=(const GameServerSectionThread& other) = delete;
	GameServerSectionThread& operator=(GameServerSectionThread&& other) = delete;

private:
	void ThreadFunction();

public: // Member Function
	void AddSection(const std::shared_ptr<GameServerSection>& section);
	void RemoveSection(const std::shared_ptr<GameServerSection>& section);

	void StartSectionThread();
};

