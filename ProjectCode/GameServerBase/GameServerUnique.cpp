#include "PreCompile.h"
#include "GameServerUnique.h"
#include "DateTime.h"

// ���α׷��� ������ �ð��� �������� ���
// ����ð��� �������� �ð��� ���س���.
std::atomic<uint64_t> GameServerUnique::s_UniqueId = (DateTime::UtcNow().Ticks() - DateTime(2020, 1, 1, DateTimeKind::Utc).Ticks());

uint64_t GameServerUnique::GetNextUniqueId()
{
	++s_UniqueId;

	return s_UniqueId;
}