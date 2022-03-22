#pragma once																										
#include "GameServerMessage.h"																					
																													
class LoginMessage : public GameServerMessage											
{																											
public:																										
	std::string m_ID;
	std::string m_PW;
																												
public:																										
	LoginMessage()																		
		: GameServerMessage(MessageType::Login)											
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
		: GameServerMessage(MessageType::Join)											
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

