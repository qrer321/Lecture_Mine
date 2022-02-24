#pragma once
#include <GameServerBase/GameServerSerializer.h>
#include <GameServerBase/GameServerMathStruct.h>
#include "MessageTypeEnum.h"
#include "ContentsEnums.h"

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

inline GameServerMessage::GameServerMessage(MessageType type)
	: m_Type(type)
	, m_Size(-1)
{
}

inline int GameServerMessage::SizeCheck()
{
	return -1;
}

inline unsigned int GameServerMessage::DataSizeCheck(const std::string& value)
{
	return 4 + static_cast<unsigned int>(value.size());
}

template <typename DataType>
unsigned int GameServerMessage::DataSizeCheck(DataType value)
{
	return sizeof(value);
}

inline void GameServerMessage::Serialize(GameServerSerializer& serializer)
{
	serializer << static_cast<int>(m_Type);
	serializer << SizeCheck();
}

inline void GameServerMessage::Deserialize(GameServerSerializer& serializer)
{
	int type;
	serializer >> type;
	m_Type = static_cast<MessageType>(type);
	serializer >> m_Size;
}