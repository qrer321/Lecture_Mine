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
	uint64_t m_ActorIndex;
	uint64_t m_ThreadIndex;
	uint64_t m_SectionIndex;
	FVector m_Dir;
	FVector m_Pos;
																												
public:																										
	PlayerUpdateMessage()																		
		: GameServerMessage(static_cast<uint32_t>(MessageType::PlayerUpdate))					
		, m_ActorIndex()																		
		, m_ThreadIndex()																		
		, m_SectionIndex()																		
		, m_Dir()																		
		, m_Pos()																		
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
		return DataSizeCheck(m_ActorIndex) + DataSizeCheck(m_ThreadIndex) + DataSizeCheck(m_SectionIndex) + DataSizeCheck(m_Dir) + DataSizeCheck(m_Pos);
	}																											
																												
	void Serialize(GameServerSerializer& serializer) override													
	{																											
		GameServerMessage::Serialize(serializer);																
																												
		serializer << m_ActorIndex;
		serializer << m_ThreadIndex;
		serializer << m_SectionIndex;
		serializer << m_Dir;
		serializer << m_Pos;
	}																											
																												
	void Deserialize(GameServerSerializer& serializer) override													
	{																											
		GameServerMessage::Deserialize(serializer);																
																												
		serializer >> m_ActorIndex;
		serializer >> m_ThreadIndex;
		serializer >> m_SectionIndex;
		serializer >> m_Dir;
		serializer >> m_Pos;
	}																											
};																											

