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

class AIUpdateMessage : public GameServerMessage											
{																											
public:																										
	int m_ObjectID;
	int m_AIType;
	EAIUpdateType m_UpdateType;
	FVector m_Pos;
	FVector m_Dir;
																												
public:																										
	AIUpdateMessage()																		
		: GameServerMessage(MessageType::AIUpdate)											
		, m_ObjectID()																		
		, m_AIType()																		
		, m_UpdateType()																		
		, m_Pos()																		
		, m_Dir()																		
	{																											
	}																											
	~AIUpdateMessage() override = default;													
																												
	AIUpdateMessage(const AIUpdateMessage& other) = delete;				
	AIUpdateMessage(AIUpdateMessage&& other) noexcept = delete;			
																												
	AIUpdateMessage& operator=(const AIUpdateMessage& other) = delete;	
	AIUpdateMessage& operator=(AIUpdateMessage&& other) = delete;			
																												
public:																										
	int SizeCheck() override																					
	{																											
		return DataSizeCheck(m_ObjectID) + DataSizeCheck(m_AIType) + DataSizeCheck(m_UpdateType) + DataSizeCheck(m_Pos) + DataSizeCheck(m_Dir);
	}																											
																												
	void Serialize(GameServerSerializer& serializer) override													
	{																											
		GameServerMessage::Serialize(serializer);																
																												
		serializer << m_ObjectID;
		serializer << m_AIType;
		serializer.WriteEnum(m_UpdateType);
		serializer << m_Pos;
		serializer << m_Dir;
	}																											
																												
	void Deserialize(GameServerSerializer& serializer) override													
	{																											
		GameServerMessage::Deserialize(serializer);																
																												
		serializer >> m_ObjectID;
		serializer >> m_AIType;
		serializer.ReadEnum(m_UpdateType);
		serializer >> m_Pos;
		serializer >> m_Dir;
	}																											
};																											

