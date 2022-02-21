#pragma once
#include "GameServerMessage.h"

class LoginResultMessage : public GameServerMessage
{
public:
	EGameServerCode m_Code;

public:
	LoginResultMessage();
	~LoginResultMessage() override = default;

	LoginResultMessage(const LoginResultMessage& other) = delete;
	LoginResultMessage(LoginResultMessage&& other) noexcept = delete;

	LoginResultMessage& operator=(const LoginResultMessage& other) = delete;
	LoginResultMessage& operator=(LoginResultMessage&& other) = delete;

public:
	int SizeCheck() override;
	void Serialize(GameServerSerializer& serializer) override;
	void Deserialize(GameServerSerializer& serializer) override;
};

class ServerDestroyMessage : public GameServerMessage
{
public:
	ServerDestroyMessage();
	~ServerDestroyMessage() override = default;

	ServerDestroyMessage(const ServerDestroyMessage& other) = delete;
	ServerDestroyMessage(ServerDestroyMessage&& other) noexcept = delete;

	ServerDestroyMessage& operator=(const ServerDestroyMessage& other) = delete;
	ServerDestroyMessage& operator=(ServerDestroyMessage&& other) = delete;

public:
	int SizeCheck() override;
	void Serialize(GameServerSerializer& serializer) override;
	void Deserialize(GameServerSerializer& serializer) override;
};

class MonsterCreateMessage : public GameServerMessage
{
public:
	MonsterCreateMessage();
	~MonsterCreateMessage() override = default;

	MonsterCreateMessage(const MonsterCreateMessage& other) = delete;
	MonsterCreateMessage(MonsterCreateMessage&& other) noexcept = delete;

	MonsterCreateMessage& operator=(const MonsterCreateMessage& other) = delete;
	MonsterCreateMessage& operator=(MonsterCreateMessage&& other) = delete;

public:
	int SizeCheck() override;
	void Serialize(GameServerSerializer& serializer) override;
	void Deserialize(GameServerSerializer& serializer) override;
};

/////////////////////////////// LoginResultMessage ///////////////////////////////
inline LoginResultMessage::LoginResultMessage()
	: GameServerMessage(MessageType::LoginResult)
	, m_Code(EGameServerCode::MAX)
{
}

inline int LoginResultMessage::SizeCheck()
{
	return static_cast<int>(DataSizeCheck(m_Code));
}

inline void LoginResultMessage::Serialize(GameServerSerializer& serializer)
{
	GameServerMessage::Serialize(serializer);
	serializer << static_cast<int>(m_Code);
}

inline void LoginResultMessage::Deserialize(GameServerSerializer& serializer)
{
	GameServerMessage::Deserialize(serializer);

	int value;
	serializer >> value;
	m_Code = static_cast<EGameServerCode>(value);
}
////////////////////////////////////////////////////////////////////////////////



/////////////////////////////// ServerDestroyMessage ///////////////////////////////
inline ServerDestroyMessage::ServerDestroyMessage()
	: GameServerMessage(MessageType::ServerDestroy)
{
}

inline int ServerDestroyMessage::SizeCheck()
{
	return 0;
}

inline void ServerDestroyMessage::Serialize(GameServerSerializer& serializer)
{
	GameServerMessage::Serialize(serializer);
}

inline void ServerDestroyMessage::Deserialize(GameServerSerializer& serializer)
{
	GameServerMessage::Deserialize(serializer);
}
/////////////////////////////////////////////////////////////////////////////////////



/////////////////////////////// MonsterCreateMessage ///////////////////////////////
inline MonsterCreateMessage::MonsterCreateMessage()
	: GameServerMessage(MessageType::MonsterCreate)
{
}

inline int MonsterCreateMessage::SizeCheck()
{
	return 0;
}

inline void MonsterCreateMessage::Serialize(GameServerSerializer& serializer)
{
	GameServerMessage::Serialize(serializer);
}

inline void MonsterCreateMessage::Deserialize(GameServerSerializer& serializer)
{
	GameServerMessage::Deserialize(serializer);
}
/////////////////////////////////////////////////////////////////////////////////////