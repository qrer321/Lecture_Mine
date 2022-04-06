#pragma once
#include <GameServerBase/GameServerQueue.h>
#include <GameServerBase/GameServerThread.h>
#include "GameServerSection.h"

class GameServerSectionThread : GameServerThread
{
private: // Member Var
	GameServerQueue*								m_SectionTaskQueue{};
	std::shared_ptr<std::thread>					m_Thread;

	std::atomic<size_t>								m_LastIndex;
	std::atomic<size_t>								m_SectionSize;
	std::mutex										m_SectionLock;
	std::vector<std::shared_ptr<GameServerSection>> m_Sections;

public: // Default
	GameServerSectionThread();
	~GameServerSectionThread() override = default;

	GameServerSectionThread(const GameServerSectionThread& other) = delete;
	GameServerSectionThread(GameServerSectionThread&& other) noexcept = delete;

	GameServerSectionThread& operator=(const GameServerSectionThread& other) = delete;
	GameServerSectionThread& operator=(GameServerSectionThread&& other) = delete;

private:
	static void ThreadFunction(GameServerSectionThread* section_thread);

public: // Member Function
	void AddSection(const std::shared_ptr<GameServerSection>& section);
	void RemoveSection(const std::shared_ptr<GameServerSection>& section);

	void StartSectionThread();
};

