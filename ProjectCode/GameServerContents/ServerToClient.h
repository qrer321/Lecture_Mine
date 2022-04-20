#pragma once																										
#include <GameServerMessage/GameServerMessage.h>																	
#include "MessageTypeEnum.h"
#include "ContentsEnum.h"
#include "ContentsStructure.h"
																													
class LoginResultMessage : public GameServerMessage											
{																											
public:																										
	EGameServerCode m_Code;
																												
public:																										
	LoginResultMessage()																		
		: GameServerMessage(static_cast<uint32_t>(MessageType::LoginResult))					
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
		: GameServerMessage(static_cast<uint32_t>(MessageType::JoinResult))					
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

class CreateCharacterResultMessage : public GameServerMessage											
{																											
public:																										
	EGameServerCode m_Code;
	FCharacterInfo m_CharacterInfo;
																												
public:																										
	CreateCharacterResultMessage()																		
		: GameServerMessage(static_cast<uint32_t>(MessageType::CreateCharacterResult))					
		, m_Code()																		
		, m_CharacterInfo()																		
	{																											
	}																											
	~CreateCharacterResultMessage() override = default;													
																												
	CreateCharacterResultMessage(const CreateCharacterResultMessage& other) = delete;				
	CreateCharacterResultMessage(CreateCharacterResultMessage&& other) noexcept = delete;			
																												
	CreateCharacterResultMessage& operator=(const CreateCharacterResultMessage& other) = delete;	
	CreateCharacterResultMessage& operator=(CreateCharacterResultMessage&& other) = delete;			
																												
public:																										
	int SizeCheck() override																					
	{																											
		return DataSizeCheck(m_Code) + DataSizeCheck(m_CharacterInfo);
	}																											
																												
	void Serialize(GameServerSerializer& serializer) override													
	{																											
		GameServerMessage::Serialize(serializer);																
																												
		serializer.WriteEnum(m_Code);
		m_CharacterInfo.Serialize(serializer);
	}																											
																												
	void Deserialize(GameServerSerializer& serializer) override													
	{																											
		GameServerMessage::Deserialize(serializer);																
																												
		serializer.ReadEnum(m_Code);
		m_CharacterInfo.DeSerialize(serializer);
	}																											
};																											

class SelectCharacterResultMessage : public GameServerMessage											
{																											
public:																										
	EGameServerCode m_Code;
	std::string m_Nickname;
																												
public:																										
	SelectCharacterResultMessage()																		
		: GameServerMessage(static_cast<uint32_t>(MessageType::SelectCharacterResult))					
		, m_Code()																		
		, m_Nickname()																		
	{																											
	}																											
	~SelectCharacterResultMessage() override = default;													
																												
	SelectCharacterResultMessage(const SelectCharacterResultMessage& other) = delete;				
	SelectCharacterResultMessage(SelectCharacterResultMessage&& other) noexcept = delete;			
																												
	SelectCharacterResultMessage& operator=(const SelectCharacterResultMessage& other) = delete;	
	SelectCharacterResultMessage& operator=(SelectCharacterResultMessage&& other) = delete;			
																												
public:																										
	int SizeCheck() override																					
	{																											
		return DataSizeCheck(m_Code) + DataSizeCheck(m_Nickname);
	}																											
																												
	void Serialize(GameServerSerializer& serializer) override													
	{																											
		GameServerMessage::Serialize(serializer);																
																												
		serializer.WriteEnum(m_Code);
		serializer << m_Nickname;
	}																											
																												
	void Deserialize(GameServerSerializer& serializer) override													
	{																											
		GameServerMessage::Deserialize(serializer);																
																												
		serializer.ReadEnum(m_Code);
		serializer >> m_Nickname;
	}																											
};																											

class UDPReadyOKMessage : public GameServerMessage											
{																											
public:																										
	EGameServerCode m_Code;
																												
public:																										
	UDPReadyOKMessage()																		
		: GameServerMessage(static_cast<uint32_t>(MessageType::UDPReadyOK))					
		, m_Code()																		
	{																											
	}																											
	~UDPReadyOKMessage() override = default;													
																												
	UDPReadyOKMessage(const UDPReadyOKMessage& other) = delete;				
	UDPReadyOKMessage(UDPReadyOKMessage&& other) noexcept = delete;			
																												
	UDPReadyOKMessage& operator=(const UDPReadyOKMessage& other) = delete;	
	UDPReadyOKMessage& operator=(UDPReadyOKMessage&& other) = delete;			
																												
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

class InsertSectionResultMessage : public GameServerMessage											
{																											
public:																										
	EGameServerCode m_Code;
	uint64_t m_UDPPort;
	uint64_t m_ActorIndex;
	uint64_t m_ThreadIndex;
	uint64_t m_SectionIndex;
																												
public:																										
	InsertSectionResultMessage()																		
		: GameServerMessage(static_cast<uint32_t>(MessageType::InsertSectionResult))					
		, m_Code()																		
		, m_UDPPort()																		
		, m_ActorIndex()																		
		, m_ThreadIndex()																		
		, m_SectionIndex()																		
	{																											
	}																											
	~InsertSectionResultMessage() override = default;													
																												
	InsertSectionResultMessage(const InsertSectionResultMessage& other) = delete;				
	InsertSectionResultMessage(InsertSectionResultMessage&& other) noexcept = delete;			
																												
	InsertSectionResultMessage& operator=(const InsertSectionResultMessage& other) = delete;	
	InsertSectionResultMessage& operator=(InsertSectionResultMessage&& other) = delete;			
																												
public:																										
	int SizeCheck() override																					
	{																											
		return DataSizeCheck(m_Code) + DataSizeCheck(m_UDPPort) + DataSizeCheck(m_ActorIndex) + DataSizeCheck(m_ThreadIndex) + DataSizeCheck(m_SectionIndex);
	}																											
																												
	void Serialize(GameServerSerializer& serializer) override													
	{																											
		GameServerMessage::Serialize(serializer);																
																												
		serializer.WriteEnum(m_Code);
		serializer << m_UDPPort;
		serializer << m_ActorIndex;
		serializer << m_ThreadIndex;
		serializer << m_SectionIndex;
	}																											
																												
	void Deserialize(GameServerSerializer& serializer) override													
	{																											
		GameServerMessage::Deserialize(serializer);																
																												
		serializer.ReadEnum(m_Code);
		serializer >> m_UDPPort;
		serializer >> m_ActorIndex;
		serializer >> m_ThreadIndex;
		serializer >> m_SectionIndex;
	}																											
};																											

class CharacterListMessage : public GameServerMessage											
{																											
public:																										
	std::vector<FCharacterInfo> m_CharactersInfo;
																												
public:																										
	CharacterListMessage()																		
		: GameServerMessage(static_cast<uint32_t>(MessageType::CharacterList))					
		, m_CharactersInfo()																		
	{																											
	}																											
	~CharacterListMessage() override = default;													
																												
	CharacterListMessage(const CharacterListMessage& other) = delete;				
	CharacterListMessage(CharacterListMessage&& other) noexcept = delete;			
																												
	CharacterListMessage& operator=(const CharacterListMessage& other) = delete;	
	CharacterListMessage& operator=(CharacterListMessage&& other) = delete;			
																												
public:																										
	int SizeCheck() override																					
	{																											
		return DataSizeCheck(m_CharactersInfo);
	}																											
																												
	void Serialize(GameServerSerializer& serializer) override													
	{																											
		GameServerMessage::Serialize(serializer);																
																												
		serializer.WriteVector(m_CharactersInfo);
	}																											
																												
	void Deserialize(GameServerSerializer& serializer) override													
	{																											
		GameServerMessage::Deserialize(serializer);																
																												
		serializer.ReadVector(m_CharactersInfo);
	}																											
};																											

class ServerDestroyMessage : public GameServerMessage											
{																											
public:																										
																												
public:																										
	ServerDestroyMessage()																		
		: GameServerMessage(static_cast<uint32_t>(MessageType::ServerDestroy))					
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

class PlayerDestroyMessage : public GameServerMessage											
{																											
public:																										
																												
public:																										
	PlayerDestroyMessage()																		
		: GameServerMessage(static_cast<uint32_t>(MessageType::PlayerDestroy))					
	{																											
	}																											
	~PlayerDestroyMessage() override = default;													
																												
	PlayerDestroyMessage(const PlayerDestroyMessage& other) = delete;				
	PlayerDestroyMessage(PlayerDestroyMessage&& other) noexcept = delete;			
																												
	PlayerDestroyMessage& operator=(const PlayerDestroyMessage& other) = delete;	
	PlayerDestroyMessage& operator=(PlayerDestroyMessage&& other) = delete;			
																												
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
	uint64_t m_ActorIndex;
																												
public:																										
	ObjectDestroyMessage()																		
		: GameServerMessage(static_cast<uint32_t>(MessageType::ObjectDestroy))					
		, m_ActorIndex()																		
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
		return DataSizeCheck(m_ActorIndex);
	}																											
																												
	void Serialize(GameServerSerializer& serializer) override													
	{																											
		GameServerMessage::Serialize(serializer);																
																												
		serializer << m_ActorIndex;
	}																											
																												
	void Deserialize(GameServerSerializer& serializer) override													
	{																											
		GameServerMessage::Deserialize(serializer);																
																												
		serializer >> m_ActorIndex;
	}																											
};																											

class PlayersUpdateMessage : public GameServerMessage											
{																											
public:																										
	std::vector<FPlayerUpdateData> m_Data;
																												
public:																										
	PlayersUpdateMessage()																		
		: GameServerMessage(static_cast<uint32_t>(MessageType::PlayersUpdate))					
		, m_Data()																		
	{																											
	}																											
	~PlayersUpdateMessage() override = default;													
																												
	PlayersUpdateMessage(const PlayersUpdateMessage& other) = delete;				
	PlayersUpdateMessage(PlayersUpdateMessage&& other) noexcept = delete;			
																												
	PlayersUpdateMessage& operator=(const PlayersUpdateMessage& other) = delete;	
	PlayersUpdateMessage& operator=(PlayersUpdateMessage&& other) = delete;			
																												
public:																										
	int SizeCheck() override																					
	{																											
		return DataSizeCheck(m_Data);
	}																											
																												
	void Serialize(GameServerSerializer& serializer) override													
	{																											
		GameServerMessage::Serialize(serializer);																
																												
		serializer.WriteVector(m_Data);
	}																											
																												
	void Deserialize(GameServerSerializer& serializer) override													
	{																											
		GameServerMessage::Deserialize(serializer);																
																												
		serializer.ReadVector(m_Data);
	}																											
};																											

class EnemyUpdateMessage : public GameServerMessage											
{																											
public:																										
	int m_ActorIndex;
	int m_EnemyType;
	EEnemyState m_UpdateType;
	FVector4 m_Pos;
	FVector4 m_Dir;
	FVector4 m_Rot;
																												
public:																										
	EnemyUpdateMessage()																		
		: GameServerMessage(static_cast<uint32_t>(MessageType::EnemyUpdate))					
		, m_ActorIndex()																		
		, m_EnemyType()																		
		, m_UpdateType()																		
		, m_Pos()																		
		, m_Dir()																		
		, m_Rot()																		
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
		return DataSizeCheck(m_ActorIndex) + DataSizeCheck(m_EnemyType) + DataSizeCheck(m_UpdateType) + DataSizeCheck(m_Pos) + DataSizeCheck(m_Dir) + DataSizeCheck(m_Rot);
	}																											
																												
	void Serialize(GameServerSerializer& serializer) override													
	{																											
		GameServerMessage::Serialize(serializer);																
																												
		serializer << m_ActorIndex;
		serializer << m_EnemyType;
		serializer.WriteEnum(m_UpdateType);
		serializer << m_Pos;
		serializer << m_Dir;
		serializer << m_Rot;
	}																											
																												
	void Deserialize(GameServerSerializer& serializer) override													
	{																											
		GameServerMessage::Deserialize(serializer);																
																												
		serializer >> m_ActorIndex;
		serializer >> m_EnemyType;
		serializer.ReadEnum(m_UpdateType);
		serializer >> m_Pos;
		serializer >> m_Dir;
		serializer >> m_Rot;
	}																											
};																											

