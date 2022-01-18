#include "PreCompile.h"
#include "GameServerUnique.h"
#include "DateTime.h"

// 프로그램이 켜지는 시간을 기준으로 잡고
// 현재시간을 기준으로 시간을 구해낸다.
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

