#pragma once

enum class EGameObjectType
{
	Player,
	Enemy,
	Item,
	MAX
};

enum class EEnemyState
{
	NONE,
	State_Idle,
	State_Trace,
	State_Attack,
	MAX
};