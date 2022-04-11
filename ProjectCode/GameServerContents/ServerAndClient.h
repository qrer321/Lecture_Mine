#pragma once																										
#include <GameServerMessage/GameServerMessage.h>																	
#include "MessageTypeEnum.h"
#include "ContentsEnum.h"
#include "ContentsStructure.h"
																													
class ChatMessage : public GameServerMessage											
{																											
public:																										
	std::string m_ID;
	std::string m_Message;
																												
public:																										
	ChatMessage()																		
		: GameServerMessage(static_cast<uint32_t>(MessageType::Chat))					
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

class PlayerUpdateMessage : public GameServerMessage											
{																											
public:																										
	FPlayerUpdateData m_Datum;
																												
public:																										
	PlayerUpdateMessage()																		
		: GameServerMessage(static_cast<uint32_t>(MessageType::PlayerUpdate))					
		, m_Datum()																		
	{																											
	}																											
	~PlayerUpdateMessage() override = default;													
																												
	PlayerUpdateMessage(const PlayerUpdateMessage& other) = delete;				
	PlayerUpdateMessage(PlayerUpdateMessage&& other) noexcept = delete;			
																												
	PlayerUpdateMessage& operator=(const PlayerUpdateMessage& other) = delete;	
	PlayerUpdateMessage& operator=(PlayerUpdateMessage&& other) = delete;			
																												
public:																										
	int SizeCheck() override																					
	{																											
		return DataSizeCheck(m_Datum);
	}																											
																												
	void Serialize(GameServerSerializer& serializer) override													
	{																											
		GameServerMessage::Serialize(serializer);																
																												
		m_Datum.Serialize(serializer);
	}																											
																												
	void Deserialize(GameServerSerializer& serializer) override													
	{																											
		GameServerMessage::Deserialize(serializer);																
																												
		m_Datum.DeSerialize(serializer);
	}																											
};																											

