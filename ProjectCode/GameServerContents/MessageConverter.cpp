#include "PreCompile.h"														
#include <GameServerMessage/MessageConverter.h>								
#include "MessageTypeEnum.h"												
#include "ServerAndClient.h"												
#include "ServerToClient.h"													
#include "ClientToServer.h"													
																				
MessageConverter::MessageConverter(const std::vector<unsigned char>& buffer)	
	: m_Buffer(buffer)															
{																				
	GameServerSerializer serializer = GameServerSerializer(buffer);				
	MessageType type;															
																				
	memcpy_s(&type, sizeof(MessageType), &buffer[0], sizeof(MessageType));		
	switch (type)																
	{																			
	case MessageType::Chat:
		m_Message = std::make_shared<ChatMessage>();
		break;
	case MessageType::PlayerUpdate:
		m_Message = std::make_shared<PlayerUpdateMessage>();
		break;
	case MessageType::LoginResult:
		m_Message = std::make_shared<LoginResultMessage>();
		break;
	case MessageType::JoinResult:
		m_Message = std::make_shared<JoinResultMessage>();
		break;
	case MessageType::CreateCharacterResult:
		m_Message = std::make_shared<CreateCharacterResultMessage>();
		break;
	case MessageType::SelectCharacterResult:
		m_Message = std::make_shared<SelectCharacterResultMessage>();
		break;
	case MessageType::InsertSectionResult:
		m_Message = std::make_shared<InsertSectionResultMessage>();
		break;
	case MessageType::CharacterList:
		m_Message = std::make_shared<CharacterListMessage>();
		break;
	case MessageType::ServerDestroy:
		m_Message = std::make_shared<ServerDestroyMessage>();
		break;
	case MessageType::PlayerDestroy:
		m_Message = std::make_shared<PlayerDestroyMessage>();
		break;
	case MessageType::ObjectDestroy:
		m_Message = std::make_shared<ObjectDestroyMessage>();
		break;
	case MessageType::PlayersUpdate:
		m_Message = std::make_shared<PlayersUpdateMessage>();
		break;
	case MessageType::EnemyUpdate:
		m_Message = std::make_shared<EnemyUpdateMessage>();
		break;
	case MessageType::Login:
		m_Message = std::make_shared<LoginMessage>();
		break;
	case MessageType::Join:
		m_Message = std::make_shared<JoinMessage>();
		break;
	case MessageType::CreateCharacter:
		m_Message = std::make_shared<CreateCharacterMessage>();
		break;
	case MessageType::SelectCharacter:
		m_Message = std::make_shared<SelectCharacterMessage>();
		break;
	case MessageType::ClientToReady:
		m_Message = std::make_shared<ClientToReadyMessage>();
		break;
	default:																	
		return;																	
	}																			
																				
	if (nullptr == m_Message)													
	{																			
		return;																	
	}																			
																				
	m_Message->Deserialize(serializer);											
}