#include "PreCompile.h"
#include "Messages.h"

/////////////////////////////// GameServerMessage ///////////////////////////////
GameServerMessage::GameServerMessage(MessageType type)
	: m_Type(type)
	, m_Size(-1)
{
}

int GameServerMessage::SizeCheck()
{
	return -1;
}

unsigned int GameServerMessage::DataSizeCheck(const std::string& value)
{
	return 4 + static_cast<unsigned int>(value.size());
}

template <typename DataType>
unsigned int GameServerMessage::DataSizeCheck(DataType value)
{
	return sizeof(value);
}

void GameServerMessage::Serialize(GameServerSerializer& serializer)
{
	serializer << static_cast<int>(m_Type);
	serializer << SizeCheck();
}

void GameServerMessage::Deserialize(GameServerSerializer& serializer)
{
	int type;
	serializer >> type;
	m_Type = static_cast<MessageType>(type);
	serializer >> m_Size;
}


/////////////////////////////// LoginMessage ///////////////////////////////
LoginMessage::LoginMessage()
	: GameServerMessage(MessageType::Login)
{
}

int LoginMessage::SizeCheck()
{
	return static_cast<int>(DataSizeCheck(m_ID) + DataSizeCheck(m_PW));
}

void LoginMessage::Serialize(GameServerSerializer& serializer)
{
	// 어떤 메시지인지 판별
	// -> LoginMessage라는 Type 정보와 LoginMessage의 크기 정보를 Serialize 한다
	GameServerMessage::Serialize(serializer);

	// 이후 ID와 PW에 해당하는 string 정보를 Serialize 한다
	serializer << m_ID;
	serializer << m_PW;
}

void LoginMessage::Deserialize(GameServerSerializer& serializer)
{
	// serializer의 정보를 GameServerMessage의 m_Type과 m_Size에 등록
	GameServerMessage::Deserialize(serializer);

	serializer >> m_ID;
	serializer >> m_PW;
}


/////////////////////////////// LoginResultMessage ///////////////////////////////
LoginResultMessage::LoginResultMessage()
	: GameServerMessage(MessageType::LoginResult)
	, m_Code(EGameServerCode::MAX)
{
}

int LoginResultMessage::SizeCheck()
{
	return static_cast<int>(DataSizeCheck(m_Code));
}

void LoginResultMessage::Serialize(GameServerSerializer& serializer)
{
	GameServerMessage::Serialize(serializer);
	serializer << static_cast<int>(m_Code);
}

void LoginResultMessage::Deserialize(GameServerSerializer& serializer)
{
	GameServerMessage::Deserialize(serializer);

	int value;
	serializer >> value;
	m_Code = static_cast<EGameServerCode>(value);
}


/////////////////////////////// ChatMessage ///////////////////////////////
ChatMessage::ChatMessage()
	: GameServerMessage(MessageType::Chat)
{
}

int ChatMessage::SizeCheck()
{
	return static_cast<int>(DataSizeCheck(m_ID) + DataSizeCheck(m_Message));
}

void ChatMessage::Serialize(GameServerSerializer& serializer)
{
	GameServerMessage::Serialize(serializer);
	serializer << m_ID;
	serializer << m_Message;
}

void ChatMessage::Deserialize(GameServerSerializer& serializer)
{
	GameServerMessage::Deserialize(serializer);
	serializer >> m_ID;
	serializer >> m_Message;
}
