#pragma once
#include "GameServerActor.h"

class Monster : public GameServerActor
{
public:

public: // Default
	Monster() = default;
	~Monster() = default;

	Monster(const Monster& other) = delete;
	Monster(Monster&& other) noexcept = delete;

	Monster& operator=(const Monster& other) = delete;
	Monster& operator=(Monster&& other) = delete;

public: // Member Function
};

