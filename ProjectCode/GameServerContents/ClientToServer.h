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

class SelectCharacterMessage : public GameServerMessage											
{																											
public:																										
	std::string m_Nickname;
																												
public:																										
	SelectCharacterMessage()																		
		: GameServerMessage(static_cast<uint32_t>(MessageType::SelectCharacter))					
		, m_Nickname()																		
	{																											
	}																											
	~SelectCharacterMessage() override = default;													
																												
	SelectCharacterMessage(const SelectCharacterMessage& other) = delete;				
	SelectCharacterMessage(SelectCharacterMessage&& other) noexcept = delete;			
																												
	SelectCharacterMessage& operator=(const SelectCharacterMessage& other) = delete;	
	SelectCharacterMessage& operator=(SelectCharacterMessage&& other) = delete;			
																												
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

class ClientToReadyMessage : public GameServerMessage											
{																											
public:																										
	uint64_t m_ActorIndex;
	uint64_t m_ThreadIndex;
	uint64_t m_SectionIndex;
																												
public:																										
	ClientToReadyMessage()																		
		: GameServerMessage(static_cast<uint32_t>(MessageType::ClientToReady))					
		, m_ActorIndex()																		
		, m_ThreadIndex()																		
		, m_SectionIndex()																		
	{																											
	}																											
	~ClientToReadyMessage() override = default;													
																												
	ClientToReadyMessage(const ClientToReadyMessage& other) = delete;				
	ClientToReadyMessage(ClientToReadyMessage&& other) noexcept = delete;			
																												
	ClientToReadyMessage& operator=(const ClientToReadyMessage& other) = delete;	
	ClientToReadyMessage& operator=(ClientToReadyMessage&& other) = delete;			
																												
public:																										
	int SizeCheck() override																					
	{																											
		return DataSizeCheck(m_ActorIndex) + DataSizeCheck(m_ThreadIndex) + DataSizeCheck(m_SectionIndex);
	}																											
																												
	void Serialize(GameServerSerializer& serializer) override													
	{																											
		GameServerMessage::Serialize(serializer);																
																												
		serializer << m_ActorIndex;
		serializer << m_ThreadIndex;
		serializer << m_SectionIndex;
	}																											
																												
	void Deserialize(GameServerSerializer& serializer) override													
	{																											
		GameServerMessage::Deserialize(serializer);																
																												
		serializer >> m_ActorIndex;
		serializer >> m_ThreadIndex;
		serializer >> m_SectionIndex;
	}																											
};																											

class UDPConnectResultMessage : public GameServerMessage											
{																											
public:																										
	uint64_t m_ActorIndex;
	uint64_t m_ThreadIndex;
	uint64_t m_SectionIndex;
																												
public:																										
	UDPConnectResultMessage()																		
		: GameServerMessage(static_cast<uint32_t>(MessageType::UDPConnectResult))					
		, m_ActorIndex()																		
		, m_ThreadIndex()																		
		, m_SectionIndex()																		
	{																											
	}																											
	~UDPConnectResultMessage() override = default;													
																												
	UDPConnectResultMessage(const UDPConnectResultMessage& other) = delete;				
	UDPConnectResultMessage(UDPConnectResultMessage&& other) noexcept = delete;			
																												
	UDPConnectResultMessage& operator=(const UDPConnectResultMessage& other) = delete;	
	UDPConnectResultMessage& operator=(UDPConnectResultMessage&& other) = delete;			
																												
public:																										
	int SizeCheck() override																					
	{																											
		return DataSizeCheck(m_ActorIndex) + DataSizeCheck(m_ThreadIndex) + DataSizeCheck(m_SectionIndex);
	}																											
																												
	void Serialize(GameServerSerializer& serializer) override													
	{																											
		GameServerMessage::Serialize(serializer);																
																												
		serializer << m_ActorIndex;
		serializer << m_ThreadIndex;
		serializer << m_SectionIndex;
	}																											
																												
	void Deserialize(GameServerSerializer& serializer) override													
	{																											
		GameServerMessage::Deserialize(serializer);																
																												
		serializer >> m_ActorIndex;
		serializer >> m_ThreadIndex;
		serializer >> m_SectionIndex;
	}																											
};																											

class MoveLevelResponseMessage : public GameServerMessage											
{																											
public:																										
	uint64_t m_ActorIndex;
	uint64_t m_ThreadIndex;
	uint64_t m_SectionIndex;
																												
public:																										
	MoveLevelResponseMessage()																		
		: GameServerMessage(static_cast<uint32_t>(MessageType::MoveLevelResponse))					
		, m_ActorIndex()																		
		, m_ThreadIndex()																		
		, m_SectionIndex()																		
	{																											
	}																											
	~MoveLevelResponseMessage() override = default;													
																												
	MoveLevelResponseMessage(const MoveLevelResponseMessage& other) = delete;				
	MoveLevelResponseMessage(MoveLevelResponseMessage&& other) noexcept = delete;			
																												
	MoveLevelResponseMessage& operator=(const MoveLevelResponseMessage& other) = delete;	
	MoveLevelResponseMessage& operator=(MoveLevelResponseMessage&& other) = delete;			
																												
public:																										
	int SizeCheck() override																					
	{																											
		return DataSizeCheck(m_ActorIndex) + DataSizeCheck(m_ThreadIndex) + DataSizeCheck(m_SectionIndex);
	}																											
																												
	void Serialize(GameServerSerializer& serializer) override													
	{																											
		GameServerMessage::Serialize(serializer);																
																												
		serializer << m_ActorIndex;
		serializer << m_ThreadIndex;
		serializer << m_SectionIndex;
	}																											
																												
	void Deserialize(GameServerSerializer& serializer) override													
	{																											
		GameServerMessage::Deserialize(serializer);																
																												
		serializer >> m_ActorIndex;
		serializer >> m_ThreadIndex;
		serializer >> m_SectionIndex;
	}																											
};																											

