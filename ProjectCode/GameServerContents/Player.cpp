#include "PreCompile.h"
#include "Player.h"

#include <GameServerNet/TCPSession.h>
#include <GameServerCore/GameServerSection.h>
#include "ServerToClient.h"
#include "ClientToServer.h"
#include "ContentsSystemEnum.h"

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
}

bool Player::InsertSection()
{
	if (nullptr == m_UserData)
	{
		GameServerDebug::AssertDebugMsg("User Data Is Not Set");
		return false;
	}

	InsertSectionResultMessage insert_message;
	GameServerSerializer serializer;
	insert_message.m_Code = EGameServerCode::OK;
	insert_message.Serialize(serializer);
	GetSession()->Send(serializer.GetData());

	GetSection()->Broadcasting();
	GameServerDebug::LogInfo("Insert Section Result Send");

	return true;
}
