#include "PreCompile.h"
#include "Player.h"

#include <GameServerNet/TCPSession.h>
#include <GameServerCore/GameServerSection.h>
#include "ServerToClient.h"
#include "ClientToServer.h"
#include "ContentsUserData.h"
#include "ContentsSystemEnum.h"

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
	GetSession()->Send(GetPlayerUpdateSerializer().GetData());
}

void Player::PlayerUpdateBroadcasting()
{
	GetSection()->Broadcasting(GetPlayerUpdateSerializer().GetData(), GetActorIndex());
}

void Player::SessionInitialize()
{
	m_UserData = GetSession()->GetLink<ContentsUserData>(EDataIndex::USER_DATA);
	if (nullptr == m_UserData)
	{
		GameServerDebug::AssertDebugMsg("User Data Setting Error");
		return;
	}
}

void Player::SectionInitialize()
{
	//GetSection();
}

void Player::Update(float delta_time)
{
	CheckMessage();
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
	insert_message.m_ActorIndex = GetActorIndex();
	insert_message.m_ThreadIndex = GetThreadIndex();
	insert_message.m_SectionIndex = GetSectionIndex();
	insert_message.Serialize(serializer);
	GetSession()->Send(serializer.GetData());

	//PlayerUpdateBroadcasting();
	GameServerDebug::LogInfo("Insert Section Result Send");

	return true;
}
