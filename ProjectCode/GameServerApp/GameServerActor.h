#pragma once

class GameServerActor
{
public:

public: // Default
	GameServerActor() = default;
	~GameServerActor() = default;

	GameServerActor(const GameServerActor& other) = delete;
	GameServerActor(GameServerActor&& other) noexcept = delete;

	GameServerActor& operator=(const GameServerActor& other) = delete;
	GameServerActor& operator=(GameServerActor&& other) = delete;

public: // Member Function
};