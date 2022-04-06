#pragma once
#include "GameServerObjectBase.h"
#include <string>

class GameServerNameBase
{
private: // Member Var
	std::string m_Name;

public: // Default
	GameServerNameBase() = default;
	virtual ~GameServerNameBase() = default;

	GameServerNameBase(const GameServerNameBase& other) = delete;
	GameServerNameBase(GameServerNameBase&& other) noexcept = default;

public:
	GameServerNameBase& operator=(const GameServerNameBase& other) = delete;
	GameServerNameBase& operator=(GameServerNameBase&& other) = delete;

public:
	virtual void SetName(const std::string& name);

	[[nodiscard]] std::string_view GetNameView() const;
	[[nodiscard]] std::string GetNameCopy() const;
	[[nodiscard]] const char* GetNamePtr() const;

private:

public: // Member Function
};

