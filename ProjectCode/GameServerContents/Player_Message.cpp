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
	GetSession()->Send(serializer.GetData());
	PlayerUpdateBroadcasting();

	PlayerUpdateSelf();
}