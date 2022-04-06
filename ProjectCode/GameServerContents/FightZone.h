#pragma once
#include <GameServerCore/GameServerSection.h>

class FightZone : public GameServerSection
{
private: // Member Var

public: // Default
	FightZone() = default;
	~FightZone() = default;

	FightZone(const FightZone& other) = delete;
	FightZone(FightZone&& other) noexcept = delete;
	FightZone& operator=(const FightZone& other) = delete;
	FightZone& operator=(FightZone&& other) = delete;

private:

public: // Member Function
};

