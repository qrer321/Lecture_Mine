#pragma once
#include <GameServerBase/GameServerSerializer.h>
#include "MessageTypeEnum.h"


enum class EGameServerCode
{
	OK,
	LoginError,
	MAX,
};

class GameServerMessage
{
private: // Member Var
	MessageType		m_Type;
	unsigned int	m_Size;

public: // Default
	GameServerMessage() = delete;
	explicit GameServerMessage(MessageType type);
	virtual ~GameServerMessage() = default;

	GameServerMessage(const GameServerMessage& other) = delete;
	GameServerMessage(GameServerMessage&& other) noexcept = delete;

	GameServerMessage& operator=(const GameServerMessage& other) = delete;
	GameServerMessage& operator=(GameServerMessage&& other) = delete;

public:
	[[nodiscard]] MessageType GetType() const { return m_Type; }

public: // Member Function
	virtual int SizeCheck() = 0;
	unsigned int DataSizeCheck(const std::string& value);

	template<typename DataType>
	unsigned int DataSizeCheck(DataType value);

	virtual void Serialize(GameServerSerializer& serializer);
	virtual void Deserialize(GameServerSerializer& serializer);
};

class LoginMessage : public GameServerMessage
{
public:
	std::string m_ID;
	std::string m_PW;

public:
	LoginMessage();
	~LoginMessage() override = default;

	LoginMessage(const LoginMessage& other) = delete;
	LoginMessage(LoginMessage&& other) noexcept = delete;

	LoginMessage& operator=(const LoginMessage& other) = delete;
	LoginMessage& operator=(LoginMessage&& other) = delete;

public:
	int SizeCheck() override;
	void Serialize(GameServerSerializer& serializer) override;
	void Deserialize(GameServerSerializer& serializer) override;
};

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

class ChatMessage : public GameServerMessage
{
public:
	std::string m_ID;
	std::string m_Message;

public:
	ChatMessage();
	~ChatMessage() override = default;

	ChatMessage(const ChatMessage& other) = delete;
	ChatMessage(ChatMessage&& other) noexcept = delete;

	ChatMessage& operator=(const ChatMessage& other) = delete;
	ChatMessage& operator=(ChatMessage&& other) = delete;

public:
	int SizeCheck() override;
	void Serialize(GameServerSerializer& serializer) override;
	void Deserialize(GameServerSerializer& serializer) override;
};