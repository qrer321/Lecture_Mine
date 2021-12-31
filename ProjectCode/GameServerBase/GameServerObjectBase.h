#pragma once

// �뵵 : 
// �з� :
// ÷�� : 
class GameServerObjectBase
{
private: // Member Var

public: // Default
	GameServerObjectBase();
	~GameServerObjectBase();

	GameServerObjectBase(const GameServerObjectBase& _Other) = delete;
	GameServerObjectBase(GameServerObjectBase&& _Other) noexcept;

protected:
	GameServerObjectBase& operator=(const GameServerObjectBase& _Other) = delete;
	GameServerObjectBase& operator=(GameServerObjectBase&& _Other) = delete;

private:

public: // Member Function
	bool IsValid();
};

