#include "PreCompile.h"
#include "GameServerContentsCore.h"
#include <GameServerNet/TCPSession.h>
#include <GameServerMessage/MessageConverter.h>
#include <GameServerCore/GameServerSectionManager.h>
#include "ServerDispatcher.h"
#include "ContentsSystemEnum.h"
#include "ContentsEnum.h"
#include "ContentsUserData.h"
#include "ContentsPlayerData.h"
#include "FightZone.h"
#include "NoneFightZone.h"
#include "Player.h"

void GameServerContentsCore::UserStart()
{
	DispatcherRegistration();

	SetAcceptCallback(AcceptEvent);
}

void GameServerContentsCore::AcceptEvent(const std::shared_ptr<TCPSession>& session)
{
	session->SetRecvCallBack(RecvEvent);
	session->SetCloseCallBack(CloseEvent);

	const std::string logText = std::to_string(static_cast<int>(session->GetSocket()));
	GameServerDebug::LogInfo(logText + " Socket Connected");


	GameServerSectionManager::GetInst()->Init(3, "SectionThread");

	GameServerSectionManager::GetInst()->CreateSection<NoneFightZone>(0, ESectionType::NONE_FIGHT);

	GameServerSectionManager::GetInst()->CreateSection<FightZone>(1, ESectionType::FIGHT_ZONE_1);
	GameServerSectionManager::GetInst()->CreateSection<FightZone>(1, ESectionType::FIGHT_ZONE_2);
	GameServerSectionManager::GetInst()->CreateSection<FightZone>(2, ESectionType::FIGHT_ZONE_3);
	GameServerSectionManager::GetInst()->CreateSection<FightZone>(2, ESectionType::FIGHT_ZONE_4);
}

void GameServerContentsCore::RecvEvent(const std::shared_ptr<TCPSession>& session, const std::vector<unsigned char>& value)
{
	MessageConverter converter = MessageConverter(value);
	if (false == converter.IsValid())
	{
		// converter�� ��ȿ���� �ʴٸ�
		// Ŭ���̾�Ʈ�� ������ ���´�.
		GameServerDebug::AssertDebugMsg("Invalid Message");
		return;
	}

	// n�� ���� ������ ������ Ŀ�ؼǵ� ���� Ŀ�ؼ��̶� ���� �߶󳻾�� �Ѵ�.
	// -> �޽��� ó���� �ð��� ����Ͽ��� �Ѵ�.

	MessageHandler<TCPSession> handler;
	if (false == g_dispatcher.GetHandler(converter.GetMessageType(), handler))
	{
		GameServerDebug::AssertDebugMsg("Invalid Handler");
		return;
	}

	handler(session, std::move(converter.GetServerMessage()));
}

void GameServerContentsCore::CloseEvent(const std::shared_ptr<TCPSession>& session)
{
	const std::string logText = std::to_string(static_cast<int>(session->GetSocket()));
	GameServerDebug::LogInfo(logText + " Connection has Ended");

	const std::shared_ptr<ContentsUserData> user_data = session->GetLink<ContentsUserData>(EDataIndex::USER_DATA);
	if (nullptr != user_data)
	{
		// �α����� �ߴ�.
	}

	const std::shared_ptr<ContentsPlayerData> player_data = session->GetLink<ContentsPlayerData>(EDataIndex::PLAYABLE);
	if (nullptr != player_data)
	{
		// Section���� �����Ͽ� �÷����ߴ� ������.
		Player* connect_player = player_data->m_ConnectPlayer;
		if (nullptr != connect_player)
		{
			connect_player->SetDeath(true);
		}
	}

	session->ClearLinkObject();
}