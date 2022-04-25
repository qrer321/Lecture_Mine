#include "PreCompile.h"
#include "Player.h"

#include <GameServerNet/TCPSession.h>
#include <GameServerMessage/GameServerMessage.h>
#include <GameServerCore/GameServerSection.h>
#include "ServerToClient.h"
#include "ClientToServer.h"
#include "MessageTypeEnum.h"

void Player::CheckMessage()
{
	while (false == IsEmptyMessage())
	{
		std::shared_ptr<GameServerMessage> message = PopMessage();

		const MessageType message_type = message->GetType<MessageType>();
		switch(message_type)
		{
		case MessageType::PlayerUpdate:
			PlayerUpdateMessageProcess(std::dynamic_pointer_cast<PlayerUpdateMessage>(message));
			break;

		case MessageType::ClientToReady:
			ClientToReadyMessageProcess(std::dynamic_pointer_cast<ClientToReadyMessage>(message));
			break;

		default:
			GameServerDebug::AssertDebugMsg("Messages Received UnProcessable");
			break;
		}
	}
}

void Player::ClientToReadyMessageProcess(const std::shared_ptr<ClientToReadyMessage>& message)
{
	const std::list<std::shared_ptr<GameServerActor>>& all_playable_actor = GetSection()->GetPlayableActor();

	PlayersUpdateMessage update_message;
	GameServerSerializer serializer;

	for (const auto& actor : all_playable_actor)
	{
		if (GetActorIndex() == actor->GetActorIndex())
		{
			// 자신 제외
			continue;
		}

		const std::shared_ptr<Player>& other_player = std::dynamic_pointer_cast<Player>(actor);
		other_player->GetPlayerUpdateSerializer();

		update_message.m_Data.push_back(other_player->m_UpdateMessage.m_Datum);
	}

	// 자신을 제외한 액터들에게 메시지 Serialize 후 Send
	update_message.Serialize(serializer);
	GetTCPSession()->Send(serializer.GetData());
	PlayerUpdateBroadcasting();

	PlayerUpdateSelf();
}

void Player::PlayerUpdateMessageProcess(const std::shared_ptr<PlayerUpdateMessage>& message)
{
	if (false == m_UDPReady)
	{
		// 현재 message가 처음 날아왔다면
		m_UDPReady = true;

		UDPReadyOKMessage udp_ready_message;
		GameServerSerializer serializer;
		udp_ready_message.m_Code = EGameServerCode::OK;
		udp_ready_message.m_ActorIndex = GetActorIndex();
		udp_ready_message.Serialize(serializer);

		GetTCPSession()->Send(serializer.GetData());
		return;
	}

	message->m_Datum = m_UpdateMessage.m_Datum;

	SetActorPos(m_UpdateMessage.m_Datum.m_Pos);
	SetActorDir(m_UpdateMessage.m_Datum.m_Dir);
	PlayerUpdateBroadcasting();
}
