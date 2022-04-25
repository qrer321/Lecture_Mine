#pragma once

enum class EGameServerCode
{
	OK,
	LoginError,
	JoinError,
	CreateCharacterError,
	SelectCharacterError,
	MAX,
};

enum class EGameObjectType
{
	Player,
	Enemy,
	Item,
	Portal,
	MAX
};

enum class EPlayerState
{
	NONE,
	State_Idle,
	State_Move,
	State_Attack,
	MAX,
};

enum class EEnemyState
{
	NONE,
	State_Idle,
	State_Trace,
	State_Attack,
	MAX,
};

enum class ESectionType
{
	NONE,
	NONE_FIGHT,
	FIGHT_ZONE_1,
	FIGHT_ZONE_2,
	FIGHT_ZONE_3,
	FIGHT_ZONE_4,
	MAX,
};