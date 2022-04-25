#pragma once
#include <GameServerCore/GameServerSection.h>

class FightZone : public GameServerSection
{
private: // Member Var

public: // Default
	FightZone() = default;
	~FightZone() override = default;

	FightZone(const FightZone& other) = delete;
	FightZone(FightZone&& other) noexcept = delete;
	FightZone& operator=(const FightZone& other) = delete;
	FightZone& operator=(FightZone&& other) = delete;

private:

public: // Member Function
	void UserUpdate() override;
	bool Init() override;
};

