#pragma once
#include <string>
#include <GameServerBase/GameServerObjectBase.h>

class GameServerUser : public GameServerObjectBase
{
public:
	std::string m_ID;

public: // Default
	GameServerUser() = default;
	~GameServerUser() override = default;

	GameServerUser(const GameServerUser& other) = delete;
	GameServerUser(GameServerUser&& other) noexcept = delete;

	GameServerUser& operator=(const GameServerUser& other) = delete;
	GameServerUser& operator=(GameServerUser&& other) = delete;

public: // Member Function
};

