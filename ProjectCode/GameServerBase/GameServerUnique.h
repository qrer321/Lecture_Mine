#pragma once

// 용도 : 
// 분류 :
// 첨언 : 
class GameServerUnique
{
private: // static Var
	static std::atomic<__int64> s_UniqueId;

public:	// static Function
	static __int64 GetNextUniqueId();

public: // Default
	GameServerUnique();
	~GameServerUnique();

	GameServerUnique(const GameServerUnique& other) = delete;
	GameServerUnique(GameServerUnique&& other) noexcept;

public:
	GameServerUnique& operator=(const GameServerUnique& other) = delete;
	GameServerUnique& operator=(GameServerUnique&& other) = delete;

private:

public: // Member Function
	
};

