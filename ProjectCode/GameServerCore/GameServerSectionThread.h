#pragma once
#include <GameServerBase/GameServerQueue.h>
#include "GameServerSection.h"

// 용도 : 
// 분류 :
// 첨언 : 
class GameServerSectionThread
{
private: // Member Var
	GameServerQueue*				m_SectionTaskQueue{};

	std::shared_ptr<std::thread>	m_Thread;
	std::vector<GameServerSection*> m_Sections;

public: // Default
	GameServerSectionThread() = default;
	~GameServerSectionThread() = default;

	GameServerSectionThread(const GameServerSectionThread& other) = delete;
	GameServerSectionThread(GameServerSectionThread&& other) noexcept = delete;

	GameServerSectionThread& operator=(const GameServerSectionThread& other) = delete;
	GameServerSectionThread& operator=(GameServerSectionThread&& other) = delete;

private:
	static void ThreadFunction(GameServerSectionThread* thread);

public: // Member Function
	void AddSection(GameServerSection* section);
	void RemoveSection(GameServerSection* section);
};

