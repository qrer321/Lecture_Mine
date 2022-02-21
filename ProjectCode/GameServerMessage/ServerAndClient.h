#pragma once
#include "GameServerMessage.h"

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



/////////////////////////////// ChatMessage ///////////////////////////////
inline ChatMessage::ChatMessage()
	: GameServerMessage(MessageType::Chat)
{
}

inline int ChatMessage::SizeCheck()
{
	return static_cast<int>(DataSizeCheck(m_ID) + DataSizeCheck(m_Message));
}

inline void ChatMessage::Serialize(GameServerSerializer& serializer)
{
	GameServerMessage::Serialize(serializer);
	serializer << m_ID;
	serializer << m_Message;
}

inline void ChatMessage::Deserialize(GameServerSerializer& serializer)
{
	GameServerMessage::Deserialize(serializer);
	serializer >> m_ID;
	serializer >> m_Message;
}
///////////////////////////////////////////////////////////////////////////