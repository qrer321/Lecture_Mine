#pragma once
#include <random>

struct ServerVector
{
	float X;
	float Y;
	float Z;
};

class GameServerMath
{

};

class GameServerRandom
{
private:
	std::random_device	m_rd;
	std::mt19937		m_gen;

public:
	int RandomInt(int min, int max)
	{
		std::uniform_int_distribution<int> dis(min, max);
		return dis(m_gen);
	}

	float RandomFloat(float min, float max)
	{
		std::uniform_real_distribution<float> dis(min, max);
		return dis(m_gen);
	}

public:
	GameServerRandom()
		: m_gen(m_rd())
	{
		
	}

	GameServerRandom(unsigned int seed)
		: m_gen(seed)
	{
		
	}
};