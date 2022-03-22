#pragma once
#include "GameServerActor.h"

class Player : public GameServerActor
{
public:

public: // Default
	Player() = default;
	~Player() = default;

	Player(const Player& other) = delete;
	Player(Player&& other) noexcept = delete;

	Player& operator=(const Player& other) = delete;
	Player& operator=(Player&& other) = delete;

public: // Member Function
};