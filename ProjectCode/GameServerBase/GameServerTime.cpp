#include "PreCompile.h"
#include "GameServerTime.h"

void GameServerTime::Reset()
{
	m_Current = std::chrono::high_resolution_clock::now();
	m_Prev = m_Current;
}

float GameServerTime::Update()
{
	m_Current = std::chrono::high_resolution_clock::now();

	const double delta_time_double = std::chrono::duration<double>(m_Current - m_Prev).count();
	m_Prev = m_Current;

	m_DeltaTime = static_cast<float>(delta_time_double);
	return m_DeltaTime;
}
