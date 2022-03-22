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

class JoinResultMessage : public GameServerMessage											
{																											
public:																										
	EGameServerCode m_Code;
																												
public:																										
	JoinResultMessage()																		
		: GameServerMessage(MessageType::JoinResult)											
		, m_Code()																		
	{																											
	}																											
	~JoinResultMessage() override = default;													
																												
	JoinResultMessage(const JoinResultMessage& other) = delete;				
	JoinResultMessage(JoinResultMessage&& other) noexcept = delete;			
																												
	JoinResultMessage& operator=(const JoinResultMessage& other) = delete;	
	JoinResultMessage& operator=(JoinResultMessage&& other) = delete;			
																												
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

class ObjectDestroyMessage : public GameServerMessage											
{																											
public:																										
	int m_ObjectID;
																												
public:																										
	ObjectDestroyMessage()																		
		: GameServerMessage(MessageType::ObjectDestroy)											
		, m_ObjectID()																		
	{																											
	}																											
	~ObjectDestroyMessage() override = default;													
																												
	ObjectDestroyMessage(const ObjectDestroyMessage& other) = delete;				
	ObjectDestroyMessage(ObjectDestroyMessage&& other) noexcept = delete;			
																												
	ObjectDestroyMessage& operator=(const ObjectDestroyMessage& other) = delete;	
	ObjectDestroyMessage& operator=(ObjectDestroyMessage&& other) = delete;			
																												
public:																										
	int SizeCheck() override																					
	{																											
		return DataSizeCheck(m_ObjectID);
	}																											
																												
	void Serialize(GameServerSerializer& serializer) override													
	{																											
		GameServerMessage::Serialize(serializer);																
																												
		serializer << m_ObjectID;
	}																											
																												
	void Deserialize(GameServerSerializer& serializer) override													
	{																											
		GameServerMessage::Deserialize(serializer);																
																												
		serializer >> m_ObjectID;
	}																											
};																											

class EnemyUpdateMessage : public GameServerMessage											
{																											
public:																										
	int m_ObjectID;
	int m_EnemyType;
	EEnemyState m_UpdateType;
	FVector m_Pos;
	FVector m_Dir;
																												
public:																										
	EnemyUpdateMessage()																		
		: GameServerMessage(MessageType::EnemyUpdate)											
		, m_ObjectID()																		
		, m_EnemyType()																		
		, m_UpdateType()																		
		, m_Pos()																		
		, m_Dir()																		
	{																											
	}																											
	~EnemyUpdateMessage() override = default;													
																												
	EnemyUpdateMessage(const EnemyUpdateMessage& other) = delete;				
	EnemyUpdateMessage(EnemyUpdateMessage&& other) noexcept = delete;			
																												
	EnemyUpdateMessage& operator=(const EnemyUpdateMessage& other) = delete;	
	EnemyUpdateMessage& operator=(EnemyUpdateMessage&& other) = delete;			
																												
public:																										
	int SizeCheck() override																					
	{																											
		return DataSizeCheck(m_ObjectID) + DataSizeCheck(m_EnemyType) + DataSizeCheck(m_UpdateType) + DataSizeCheck(m_Pos) + DataSizeCheck(m_Dir);
	}																											
																												
	void Serialize(GameServerSerializer& serializer) override													
	{																											
		GameServerMessage::Serialize(serializer);																
																												
		serializer << m_ObjectID;
		serializer << m_EnemyType;
		serializer.WriteEnum(m_UpdateType);
		serializer << m_Pos;
		serializer << m_Dir;
	}																											
																												
	void Deserialize(GameServerSerializer& serializer) override													
	{																											
		GameServerMessage::Deserialize(serializer);																
																												
		serializer >> m_ObjectID;
		serializer >> m_EnemyType;
		serializer.ReadEnum(m_UpdateType);
		serializer >> m_Pos;
		serializer >> m_Dir;
	}																											
};																											

