#pragma once																										
#include "GameServerMessage.h"																					
																													
class ChatMessage : public GameServerMessage											
{																											
public:																										
	std::string m_ID;
	std::string m_Message;
																												
public:																										
	ChatMessage()																		
		: GameServerMessage(MessageType::Chat)											
		, m_ID()																		
		, m_Message()																		
	{																											
	}																											
	~ChatMessage() override = default;													
																												
	ChatMessage(const ChatMessage& other) = delete;				
	ChatMessage(ChatMessage&& other) noexcept = delete;			
																												
	ChatMessage& operator=(const ChatMessage& other) = delete;	
	ChatMessage& operator=(ChatMessage&& other) = delete;			
																												
public:																										
	int SizeCheck() override																					
	{																											
		return DataSizeCheck(m_ID) + DataSizeCheck(m_Message);
	}																											
																												
	void Serialize(GameServerSerializer& serializer) override													
	{																											
		GameServerMessage::Serialize(serializer);																
																												
		serializer << m_ID;
		serializer << m_Message;
	}																											
																												
	void Deserialize(GameServerSerializer& serializer) override													
	{																											
		GameServerMessage::Deserialize(serializer);																
																												
		serializer >> m_ID;
		serializer >> m_Message;
	}																											
};																											

