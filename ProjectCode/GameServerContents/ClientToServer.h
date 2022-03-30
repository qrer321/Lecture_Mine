#pragma once																										
#include <GameServerMessage/GameServerMessage.h>																	
#include "MessageTypeEnum.h"
#include "ContentsEnum.h"
#include "ContentsStructure.h"
																													
class LoginMessage : public GameServerMessage											
{																											
public:																										
	std::string m_ID;
	std::string m_PW;
																												
public:																										
	LoginMessage()																		
		: GameServerMessage(static_cast<uint32_t>(MessageType::Login))					
		, m_ID()																		
		, m_PW()																		
	{																											
	}																											
	~LoginMessage() override = default;													
																												
	LoginMessage(const LoginMessage& other) = delete;				
	LoginMessage(LoginMessage&& other) noexcept = delete;			
																												
	LoginMessage& operator=(const LoginMessage& other) = delete;	
	LoginMessage& operator=(LoginMessage&& other) = delete;			
																												
public:																										
	int SizeCheck() override																					
	{																											
		return DataSizeCheck(m_ID) + DataSizeCheck(m_PW);
	}																											
																												
	void Serialize(GameServerSerializer& serializer) override													
	{																											
		GameServerMessage::Serialize(serializer);																
																												
		serializer << m_ID;
		serializer << m_PW;
	}																											
																												
	void Deserialize(GameServerSerializer& serializer) override													
	{																											
		GameServerMessage::Deserialize(serializer);																
																												
		serializer >> m_ID;
		serializer >> m_PW;
	}																											
};																											

class JoinMessage : public GameServerMessage											
{																											
public:																										
	std::string m_ID;
	std::string m_PW;
																												
public:																										
	JoinMessage()																		
		: GameServerMessage(static_cast<uint32_t>(MessageType::Join))					
		, m_ID()																		
		, m_PW()																		
	{																											
	}																											
	~JoinMessage() override = default;													
																												
	JoinMessage(const JoinMessage& other) = delete;				
	JoinMessage(JoinMessage&& other) noexcept = delete;			
																												
	JoinMessage& operator=(const JoinMessage& other) = delete;	
	JoinMessage& operator=(JoinMessage&& other) = delete;			
																												
public:																										
	int SizeCheck() override																					
	{																											
		return DataSizeCheck(m_ID) + DataSizeCheck(m_PW);
	}																											
																												
	void Serialize(GameServerSerializer& serializer) override													
	{																											
		GameServerMessage::Serialize(serializer);																
																												
		serializer << m_ID;
		serializer << m_PW;
	}																											
																												
	void Deserialize(GameServerSerializer& serializer) override													
	{																											
		GameServerMessage::Deserialize(serializer);																
																												
		serializer >> m_ID;
		serializer >> m_PW;
	}																											
};																											

class CreateCharacterMessage : public GameServerMessage											
{																											
public:																										
	std::string m_Nickname;
																												
public:																										
	CreateCharacterMessage()																		
		: GameServerMessage(static_cast<uint32_t>(MessageType::CreateCharacter))					
		, m_Nickname()																		
	{																											
	}																											
	~CreateCharacterMessage() override = default;													
																												
	CreateCharacterMessage(const CreateCharacterMessage& other) = delete;				
	CreateCharacterMessage(CreateCharacterMessage&& other) noexcept = delete;			
																												
	CreateCharacterMessage& operator=(const CreateCharacterMessage& other) = delete;	
	CreateCharacterMessage& operator=(CreateCharacterMessage&& other) = delete;			
																												
public:																										
	int SizeCheck() override																					
	{																											
		return DataSizeCheck(m_Nickname);
	}																											
																												
	void Serialize(GameServerSerializer& serializer) override													
	{																											
		GameServerMessage::Serialize(serializer);																
																												
		serializer << m_Nickname;
	}																											
																												
	void Deserialize(GameServerSerializer& serializer) override													
	{																											
		GameServerMessage::Deserialize(serializer);																
																												
		serializer >> m_Nickname;
	}																											
};																											

