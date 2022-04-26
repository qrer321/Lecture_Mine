#include "PreCompile.h"
#include "GameServerCollision.h"
#include "GameServerSection.h"

std::function<bool(GameServerCollision*, GameServerCollision*)> GameServerCollision::s_CollisionCheckFunction[static_cast<int>(CollisionCheckType::MAX)][static_cast<int>(CollisionCheckType::MAX)];

class GameCollisionFunctionInitializer
{
public:
	GameCollisionFunctionInitializer()
	{
		GameServerCollision::s_CollisionCheckFunction[static_cast<int>(CollisionCheckType::SPHERE)][static_cast<int>(CollisionCheckType::SPHERE)] = GameServerCollision::SphereToSphere;
	}
};

GameCollisionFunctionInitializer collision_function_initializer = GameCollisionFunctionInitializer();

GameServerCollision::GameServerCollision()
	: m_OwnerSection(nullptr)
	, m_OwnerActor(nullptr)
	, m_GroupIndex(0)
{
}

bool GameServerCollision::CollisionCheck(CollisionCheckType check_type, int collision_type, CollisionCheckType other_check_type)
{
	if (nullptr == m_OwnerSection)
	{
		GameServerDebug::AssertDebugMsg("Collision Not Part Of The Section");
		return false;
	}

	if (nullptr == m_OwnerActor)
	{
		GameServerDebug::AssertDebugMsg("Collision Not Part Of The Actor");
		return false;
	}

	const std::list<GameServerCollision*> collision_list = m_OwnerSection->m_CollisionList[collision_type];
	if (0 == collision_list.size())
	{
		return false;
	}

	CollisionDataUpdate();

	for (const auto& other_collision : collision_list)
	{
		other_collision->CollisionDataUpdate();

		if (true == s_CollisionCheckFunction[static_cast<int>(check_type)][static_cast<int>(other_check_type)](this, other_collision))
		{
			return true;
		}
	}

	return false;
}

bool GameServerCollision::CollisionCheckResult(CollisionCheckType check_type, int collision_type,
	CollisionCheckType other_check_type, std::vector<GameServerCollision*>& hit_result)
{
	if (nullptr == m_OwnerSection)
	{
		GameServerDebug::AssertDebugMsg("Collision Not Part Of The Section");
		return false;
	}

	if (nullptr == m_OwnerActor)
	{
		GameServerDebug::AssertDebugMsg("Collision Not Part Of The Actor");
		return false;
	}

	const std::list<GameServerCollision*> collision_list = m_OwnerSection->m_CollisionList[collision_type];
	if (0 == collision_list.size())
	{
		return false;
	}

	CollisionDataUpdate();

	for (const auto& other_collision : collision_list)
	{
		other_collision->CollisionDataUpdate();

		if (true == s_CollisionCheckFunction[static_cast<int>(check_type)][static_cast<int>(other_check_type)](this, other_collision))
		{
			hit_result.push_back(other_collision);
		}
	}

	if (0 == hit_result.size())
	{
		return false;
	}

	return true;
}

void GameServerCollision::CollisionDataUpdate()
{
	m_CollisionData.OBB.Center = (m_OwnerActor->GetActorPos() + m_Pivot).ConvertXMFloat3();
	m_CollisionData.OBB.Extents = m_Scale.HalfVector3D().ConvertXMFloat3();
	m_CollisionData.OBB.Orientation = m_Rot.ConvertXMFloat4();
}

bool GameServerCollision::SphereToSphere(const GameServerCollision* left, const GameServerCollision* right)
{
	return left->m_CollisionData.Sphere.Intersects(right->m_CollisionData.Sphere);
}

void GameServerCollision::SetDeath(bool is_death)
{
	GameServerObjectBase::SetDeath(is_death);

	if (nullptr == m_OwnerSection)
	{
		GameServerDebug::AssertDebugMsg("Owner Section Is Nullptr");
		return;
	}

	m_OwnerSection->m_CollisionList[m_GroupIndex].remove(this);
	delete this;
}