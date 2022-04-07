#pragma once

class GameServerTime
{
private:
	std::chrono::steady_clock::time_point	m_Current;
	std::chrono::steady_clock::time_point	m_Prev;

	float m_DeltaTime{};

public: // Default
	GameServerTime() = default;
	~GameServerTime() = default;

	GameServerTime(const GameServerTime& other) = delete;
	GameServerTime(GameServerTime&& other) noexcept = delete;
	GameServerTime& operator=(const GameServerTime& other) = delete;
	GameServerTime& operator=(GameServerTime&& other) = delete;

public: // Member Function
	float GetDeltaTime() const { return m_DeltaTime; }

	void Reset();
	float Update();
};