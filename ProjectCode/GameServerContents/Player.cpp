#include "PreCompile.h"
#include "Player.h"

#include <GameServerNet/TCPSession.h>
#include <GameServerCore/GameServerSection.h>
#include <GameServerCore/GameServerCollision.h>
#include "ServerToClient.h"
#include "ClientToServer.h"
#include "ContentsUserData.h"
#include "ContentsSystemEnum.h"
#include "Portal.h"

Player::Player()
	: m_HitCollision(nullptr)
	, m_PortalForMove(nullptr)
	, m_UDPReady(false)
{
}

Player::~Player()
{
	if (nullptr != m_HitCollision)
	{
		m_HitCollision->SetDeath();
		m_HitCollision = nullptr;
	}
}

PlayerUpdateMessage& Player::GetPlayerUpdateMessage()
{
	m_UpdateMessage.m_Datum.m_Pos = GetActorPos();
	m_UpdateMessage.m_Datum.m_Dir = GetActorDir();

	return m_UpdateMessage;
}

GameServerSerializer& Player::GetPlayerUpdateSerializer()
{
	m_UpdateMessage.m_Datum.m_Pos = GetActorPos();
	m_UpdateMessage.m_Datum.m_Dir = GetActorDir();

	m_Serializer.Reset();
	m_UpdateMessage.Serialize(m_Serializer);

	return m_Serializer;
}

void Player::PlayerUpdateSelf()
{
	GetTCPSession()->Send(GetPlayerUpdateSerializer().GetData());
}

void Player::PlayerUpdateBroadcasting()
{
	GetSection()->Broadcasting_UDP(GetPlayerUpdateSerializer().GetData(), GetActorIndex());
}

void Player::TCPSessionInitialize()
{
	m_UserData = GetTCPSession()->GetLink<ContentsUserData>(EDataIndex::USER_DATA);
	if (nullptr == m_UserData)
	{
		GameServerDebug::AssertDebugMsg("User Data Setting Error");
		return;
	}
}

void Player::UDPSessionInitialize()
{
}

void Player::SectionInitialize()
{
	if (nullptr == m_HitCollision)
	{
		m_HitCollision = GetSection()->CreateCollision(ECollisionGroup::PLAYER, this);
		m_HitCollision->SetScale({ 100.f, 100.f, 100.f });
	}

	m_UDPReady = false;
}

void Player::Update(float delta_time)
{
	CheckMessage();

	if (IsFrame(10) && nullptr != m_HitCollision)
	{
		static std::vector<GameServerCollision*> hit_result;

		if (true == m_HitCollision->CollisionCheckResult(CollisionCheckType::SPHERE, ECollisionGroup::PORTAL, CollisionCheckType::SPHERE, hit_result))
		{
			m_PortalForMove = hit_result[0]->GetOwnerActorConvert<Portal>();
			if (nullptr == m_PortalForMove)
			{
				GameServerDebug::AssertDebugMsg("Portal Actor Is Nullptr");
				return;
			}

			MoveLevelMessage move_message;
			GameServerSerializer serializer;
			move_message.m_ActorIndex = GetActorIndex();
			move_message.m_ThreadIndex = GetThreadIndex();
			move_message.m_SectionIndex = GetSectionIndex();
			move_message.m_MoveLevel = m_PortalForMove->m_LinkSection->GetNameCopy();
			move_message.Serialize(serializer);

			GetSection()->Broadcasting_TCP(serializer.GetData(), GetActorIndex());
			GetTCPSession()->Send(serializer.GetData());

			m_HitCollision->SetDeath();
			m_HitCollision = nullptr;

			// GetSection()->MoveSection(DynamicCast<GameServerActor>(), portal_actor->m_LinkSection);
		}

		hit_result.clear();
	}
}

bool Player::InsertSection()
{
	if (nullptr == m_UserData)
	{
		GameServerDebug::AssertDebugMsg("User Data Is Not Set");
		return false;
	}

	m_UpdateMessage.m_Datum.m_ActorIndex = GetActorIndex();
	m_UpdateMessage.m_Datum.m_ThreadIndex = GetThreadIndex();
	m_UpdateMessage.m_Datum.m_SectionIndex = GetSectionIndex();

	SetActorPos({ 200.f, 0.f, 0.f });

	InsertSectionResultMessage insert_message;
	GameServerSerializer serializer;
	insert_message.m_Code = EGameServerCode::OK;
	insert_message.m_UDPPort = GetUDPPort();
	insert_message.m_ActorIndex = GetActorIndex();
	insert_message.m_ThreadIndex = GetThreadIndex();
	insert_message.m_SectionIndex = GetSectionIndex();
	insert_message.m_MoveLevel = GetSection()->GetNameCopy();
	insert_message.Serialize(serializer);
	GetTCPSession()->Send(serializer.GetData());

	SetActorPos(FVector4::ZeroVector);

	//PlayerUpdateBroadcasting();
	GameServerDebug::LogInfo("Insert Section Result Send");

	return true;
}

void Player::DeathEvent()
{
	{
		const std::list<std::shared_ptr<GameServerActor>>& all_playable_actor = GetSection()->GetPlayableActor();

		ObjectDestroyMessage destroy_message;
		GameServerSerializer serializer;
		destroy_message.m_ActorIndex = GetActorIndex();
		destroy_message.Serialize(serializer);

		for (const auto& other_actor : all_playable_actor)
		{
			if (GetActorIndex() == other_actor->GetActorIndex())
			{
				continue;
			}

			std::shared_ptr<Player> other_player = std::dynamic_pointer_cast<Player>(other_actor);
			other_player->GetTCPSession()->Send(serializer.GetData());
		}
	}

	PlayerDestroyMessage destroy_message;
	GameServerSerializer serializer;
	destroy_message.Serialize(serializer);
	GetTCPSession()->Send(serializer.GetData());
}

void Player::Disconnect()
{
}
