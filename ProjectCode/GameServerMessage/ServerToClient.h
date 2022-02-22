#pragma once																										
#include "GameServerMessage.h"																					
																													
class LoginResultMessage : public GameServerMessage											
{																											
public:																										
	EGameServerCode m_Code;
																												
public:																										
	LoginResultMessage()																		
		: GameServerMessage(MessageType::LoginResult)											
		, m_Code()																		
	{																											
	}																											
	~LoginResultMessage() override = default;													
																												
	LoginResultMessage(const LoginResultMessage& other) = delete;				
	LoginResultMessage(LoginResultMessage&& other) noexcept = delete;			
																												
	LoginResultMessage& operator=(const LoginResultMessage& other) = delete;	
	LoginResultMessage& operator=(LoginResultMessage&& other) = delete;			
																												
public:																										
	int SizeCheck() override																					
	{																											
		return DataSizeCheck(m_Code);
	}																											
																												
	void Serialize(GameServerSerializer& serializer) override													
	{																											
		GameServerMessage::Serialize(serializer);																
																												
		serializer.WriteEnum(m_Code);
	}																											
																												
	void Deserialize(GameServerSerializer& serializer) override													
	{																											
		GameServerMessage::Deserialize(serializer);																
																												
		serializer.ReadEnum(m_Code);
	}																											
};																											

class ServerDestroyMessage : public GameServerMessage											
{																											
public:																										
																												
public:																										
	ServerDestroyMessage()																		
		: GameServerMessage(MessageType::ServerDestroy)											
	{																											
	}																											
	~ServerDestroyMessage() override = default;													
																												
	ServerDestroyMessage(const ServerDestroyMessage& other) = delete;				
	ServerDestroyMessage(ServerDestroyMessage&& other) noexcept = delete;			
																												
	ServerDestroyMessage& operator=(const ServerDestroyMessage& other) = delete;	
	ServerDestroyMessage& operator=(ServerDestroyMessage&& other) = delete;			
																												
public:																										
	int SizeCheck() override																					
	{																											
		return 0;
	}																											
																												
	void Serialize(GameServerSerializer& serializer) override													
	{																											
		GameServerMessage::Serialize(serializer);																
																												
	}																											
																												
	void Deserialize(GameServerSerializer& serializer) override													
	{																											
		GameServerMessage::Deserialize(serializer);																
																												
	}																											
};																											

class MonsterCreateMessage : public GameServerMessage											
{																											
public:																										
																												
public:																										
	MonsterCreateMessage()																		
		: GameServerMessage(MessageType::MonsterCreate)											
	{																											
	}																											
	~MonsterCreateMessage() override = default;													
																												
	MonsterCreateMessage(const MonsterCreateMessage& other) = delete;				
	MonsterCreateMessage(MonsterCreateMessage&& other) noexcept = delete;			
																												
	MonsterCreateMessage& operator=(const MonsterCreateMessage& other) = delete;	
	MonsterCreateMessage& operator=(MonsterCreateMessage&& other) = delete;			
																												
public:																										
	int SizeCheck() override																					
	{																											
		return 0;
	}																											
																												
	void Serialize(GameServerSerializer& serializer) override													
	{																											
		GameServerMessage::Serialize(serializer);																
																												
	}																											
																												
	void Deserialize(GameServerSerializer& serializer) override													
	{																											
		GameServerMessage::Deserialize(serializer);																
																												
	}																											
};																											

