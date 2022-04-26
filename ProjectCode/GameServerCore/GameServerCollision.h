#pragma once
#include <DirectXCollision.h>
#include <DirectXCollision.inl>

#include <GameServerBase/GameServerObjectBase.h>

#include "GameServerActor.h"

enum CollisionCheckType
{
	SPHERE,
	AABB,
	OBB,
	MAX,
};

class CollisionData
{
public:
	union
	{
		DirectX::BoundingSphere			Sphere;
		DirectX::BoundingBox			AABB;
		DirectX::BoundingOrientedBox	OBB;
	};

	CollisionData() {}
};

class GameServerActor;
class GameServerSection;
class GameServerCollision : public GameServerObjectBase
{
	friend GameServerActor;
	friend GameServerSection;

private: // Member Var
	GameServerSection*	m_OwnerSection;
	GameServerActor*	m_OwnerActor;
	CollisionData		m_CollisionData;
	FVector4			m_Scale;
	FVector4			m_Rot;
	FVector4			m_Pivot;
	int					m_GroupIndex;

public:
	static bool SphereToSphere(const GameServerCollision* left, const GameServerCollision* right);

	static std::function<bool(GameServerCollision*, GameServerCollision*)> s_CollisionCheckFunction[static_cast<int>(CollisionCheckType::MAX)][static_cast<int>(CollisionCheckType::MAX)];
	
public: // Default
	GameServerCollision();
	~GameServerCollision() override = default;

	GameServerCollision(const GameServerCollision& other) = delete;
	GameServerCollision(GameServerCollision&& other) noexcept = delete;

	GameServerCollision& operator=(const GameServerCollision& other) = delete;
	GameServerCollision& operator=(GameServerCollision&& other) = delete;

private:
	void SetSection(GameServerSection* owner_section) { m_OwnerSection = owner_section; }
	void SetActor(GameServerActor* owner_actor) { m_OwnerActor = owner_actor; }

public: // Member Function
	[[nodiscard]] GameServerSection* GetOwnerSection() const { return m_OwnerSection; }
	[[nodiscard]] GameServerActor* GetOwnerActor() const { return m_OwnerActor; }

	template <typename OwnerType>
	OwnerType* GetOwnerActorConvert() const { return dynamic_cast<OwnerType*>(m_OwnerActor); }

	[[nodiscard]] FVector4 GetScale() const { return m_Scale; }
	[[nodiscard]] FVector4 GetRot() const { return m_Rot; }
	[[nodiscard]] FVector4 GetPivot() const { return m_Pivot; }

	void SetScale(const FVector4& scale) { m_Scale = scale; }
	void SetRot(const FVector4& rot) { m_Rot = rot; }
	void SetPivot(const FVector4& pivot) { m_Pivot = pivot; }
	void SetGroupIndex(int group_index) { m_GroupIndex = group_index; }

	template <typename CollisionType>
	bool CollisionCheck(CollisionCheckType check_type, CollisionType collision_type, CollisionCheckType other_check_type)
	{
		return CollisionCheck(check_type, static_cast<int>(collision_type), other_check_type);
	}

	bool CollisionCheck(CollisionCheckType check_type, int collision_type, CollisionCheckType other_check_type);

	template <typename CollisionType>
	bool CollisionCheckResult(CollisionCheckType check_type, CollisionType collision_type, CollisionCheckType other_check_type, 
		std::vector<GameServerCollision*>& hit_result)
	{
		return CollisionCheckResult(check_type, static_cast<int>(collision_type), other_check_type, hit_result);
	}

	bool CollisionCheckResult(CollisionCheckType check_type, int collision_type, CollisionCheckType other_check_type,
		std::vector<GameServerCollision*>& hit_result);

	void CollisionDataUpdate();

	void SetDeath(bool is_death = true) override;
};

