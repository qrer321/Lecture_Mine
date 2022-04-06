#pragma once
#include <GameServerCore/GameServerActor.h>

class Player : public GameServerActor
{
private: // Member Var

public: // Default
	Player() = default;
	~Player() override = default;

	Player(const Player& other) = delete;
	Player(Player&& other) noexcept = delete;
	Player& operator=(const Player& other) = delete;
	Player& operator=(Player&& other) = delete;

private:
	void Update() override;

public: // Member Function
};

