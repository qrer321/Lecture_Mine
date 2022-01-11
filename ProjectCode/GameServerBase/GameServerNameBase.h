#pragma once
#include "GameServerObjectBase.h"
#include <string>

// 용도 : 
// 분류 :
// 첨언 : 
class GameServerNameBase : public GameServerObjectBase
{
private: // Member Var
	std::string m_Name;

public: // Default
	GameServerNameBase();
	~GameServerNameBase() override;

	GameServerNameBase(const GameServerNameBase& other) = delete;
	GameServerNameBase(GameServerNameBase&& other) noexcept;

public:
	GameServerNameBase& operator=(const GameServerNameBase& other) = delete;
	GameServerNameBase& operator=(GameServerNameBase&& other) = delete;

public:
	virtual void SetName(const std::string& name);

	std::string_view GetNameView() const;
	std::string GetNameCopy() const;
	const char* GetNamePtr() const;

private:

public: // Member Function
};

