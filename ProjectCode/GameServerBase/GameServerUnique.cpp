#include "PreCompile.h"
#include "GameServerUnique.h"
#include "DateTime.h"

// ���α׷��� ������ �ð��� �������� ���
// ����ð��� �������� �ð��� ���س���.
std::atomic<__int64> GameServerUnique::s_UniqueId = (DateTime::UtcNow().Ticks() - DateTime(2020, 1, 1, DateTimeKind::Utc).Ticks());

long long GameServerUnique::GetNextUniqueId()
{
	++s_UniqueId;

	return s_UniqueId;
}

GameServerUnique::GameServerUnique()
= default;

GameServerUnique::~GameServerUnique()
= default;

GameServerUnique::GameServerUnique(GameServerUnique&& other) noexcept
{

}

