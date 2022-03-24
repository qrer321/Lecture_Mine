#pragma once
#include <GameServerBase/GameServerThread.h>
#include "GameServerSection.h"

// 용도 : 
// 분류 :
// 첨언 : 
class GameServerSectionManager
{
private: // Member Var
	std::vector<std::shared_ptr<GameServerThread>> m_SectionThread;
	std::unordered_map<uint32_t, std::shared_ptr<GameServerSection>> m_AllSection;

public: // Default
	GameServerSectionManager() = default;
	~GameServerSectionManager() = default;

	GameServerSectionManager(const GameServerSectionManager& other) = delete;
	GameServerSectionManager(GameServerSectionManager&& other) noexcept = delete;

	GameServerSectionManager& operator=(const GameServerSectionManager& other) = delete;
	GameServerSectionManager& operator=(GameServerSectionManager&& other) = delete;

private:

public: // Member Function
};

