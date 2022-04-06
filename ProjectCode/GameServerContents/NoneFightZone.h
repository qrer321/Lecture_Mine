#pragma once
#include <GameServerCore/GameServerSection.h>

class NoneFightZone : public GameServerSection
{
private: // Member Var

public: // Default
	NoneFightZone() = default;
	~NoneFightZone() = default;

	NoneFightZone(const NoneFightZone& other) = delete;
	NoneFightZone(NoneFightZone&& other) noexcept = delete;
	NoneFightZone& operator=(const NoneFightZone& other) = delete;
	NoneFightZone& operator=(NoneFightZone&& other) = delete;

private:

public: // Member Function
};

