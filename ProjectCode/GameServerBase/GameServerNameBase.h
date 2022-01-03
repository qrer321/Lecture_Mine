#pragma once
#include "GameServerObjectBase.h"
#include <string>

// �뵵 : 
// �з� :
// ÷�� : 
class GameServerNameBase : public GameServerObjectBase
{
private: // Member Var
	std::string m_Name;

public: // Default
	GameServerNameBase();
	virtual ~GameServerNameBase() override;

	GameServerNameBase(const GameServerNameBase& _Other) = delete;
	GameServerNameBase(GameServerNameBase&& _Other) noexcept;

public:
	GameServerNameBase& operator=(const GameServerNameBase& _Other) = delete;
	GameServerNameBase& operator=(GameServerNameBase&& _Other) = delete;

public:
	virtual void SetName(const std::string& name);

	std::string_view GetNameView();
	std::string GetNameCopy();
	const char* GetNamePtr();

private:

public: // Member Function
};

