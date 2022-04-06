#pragma once

// 용도 : 
// 분류 :
// 첨언 : 
class GameServerUnique
{
private: // static Var
	static std::atomic<uint64_t> s_UniqueId;

public:	// static Function
	static uint64_t GetNextUniqueId();

public: // Default
	GameServerUnique() = default;
	~GameServerUnique() = default;

	GameServerUnique(const GameServerUnique& other) = delete;
	GameServerUnique(GameServerUnique&& other) noexcept = default;

public:
	GameServerUnique& operator=(const GameServerUnique& other) = delete;
	GameServerUnique& operator=(GameServerUnique&& other) = delete;

private:

public: // Member Function
	
};

