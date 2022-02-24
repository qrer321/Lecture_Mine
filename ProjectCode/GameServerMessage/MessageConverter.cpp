#include "PreCompile.h"														
#include "MessageConverter.h"												
																				
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
	case MessageType::LoginResult:
		m_Message = std::make_shared<LoginResultMessage>();
		break;
	case MessageType::ServerDestroy:
		m_Message = std::make_shared<ServerDestroyMessage>();
		break;
	case MessageType::AIUpdate:
		m_Message = std::make_shared<AIUpdateMessage>();
		break;
	case MessageType::Login:
		m_Message = std::make_shared<LoginMessage>();
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