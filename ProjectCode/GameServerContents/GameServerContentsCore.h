#pragma once
#include <GameServerCore/GameServerCore.h>

class GameServerContentsCore
{
public: // Default
	GameServerContentsCore() = default;
	~GameServerContentsCore() = default;

	GameServerContentsCore(const GameServerContentsCore& other) = delete;
	GameServerContentsCore(GameServerContentsCore&& other) noexcept = delete;
	GameServerContentsCore& operator=(const GameServerContentsCore& other) = delete;
	GameServerContentsCore& operator=(GameServerContentsCore&& other) = delete;

public: // Member Function
	void UserStart() {}
};

