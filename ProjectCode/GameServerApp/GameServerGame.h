#pragma once

class GameServerGame
{
private:
	static GameServerGame* m_Inst;

public: // Default
	GameServerGame() = default;
	~GameServerGame() = default;

	GameServerGame(const GameServerGame& other) = delete;
	GameServerGame(GameServerGame&& other) noexcept = delete;

	GameServerGame& operator=(const GameServerGame& other) = delete;
	GameServerGame& operator=(GameServerGame&& other) = delete;

public: // Member Function
	static GameServerGame& GetInst() { return *m_Inst; }
	static void Destroy();

	bool Start();
	bool Run();
};