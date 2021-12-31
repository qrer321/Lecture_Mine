#pragma once
#include <string>
#include <assert.h>

// 용도 : 
// 분류 :
// 첨언 : 
class GameServerDebug
{
private: // Member Var

public: // Default
	GameServerDebug();
	virtual ~GameServerDebug() = 0;

	GameServerDebug(const GameServerDebug& _Other) = delete;
	GameServerDebug(GameServerDebug&& _Other) noexcept;

protected:
	GameServerDebug& operator=(const GameServerDebug& _Other) = delete;
	GameServerDebug& operator=(GameServerDebug&& _Other) = delete;

private:

public: // Member Function
	static void AssertDebug();
	static void AssertDebugMsg(const std::string& _Msg);
};

