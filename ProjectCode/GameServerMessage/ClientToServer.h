#pragma once
#include "GameServerMessage.h"

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

class QuitMessage : public GameServerMessage
{
public:
	std::string m_ID;
	std::string m_PW;

public:
	QuitMessage();
	~QuitMessage() override = default;

	QuitMessage(const QuitMessage& other) = delete;
	QuitMessage(QuitMessage&& other) noexcept = delete;

	QuitMessage& operator=(const QuitMessage& other) = delete;
	QuitMessage& operator=(QuitMessage&& other) = delete;

public:
	int SizeCheck() override;
	void Serialize(GameServerSerializer& serializer) override;
	void Deserialize(GameServerSerializer& serializer) override;
};

class AttackMessage : public GameServerMessage
{
public:
	std::string m_ID;
	std::string m_PW;

public:
	AttackMessage();
	~AttackMessage() override = default;

	AttackMessage(const AttackMessage& other) = delete;
	AttackMessage(AttackMessage&& other) noexcept = delete;

	AttackMessage& operator=(const AttackMessage& other) = delete;
	AttackMessage& operator=(AttackMessage&& other) = delete;

public:
	int SizeCheck() override;
	void Serialize(GameServerSerializer& serializer) override;
	void Deserialize(GameServerSerializer& serializer) override;
};

/////////////////////////////// LoginMessage ///////////////////////////////
inline LoginMessage::LoginMessage()
	: GameServerMessage(MessageType::Login)
{
}

inline int LoginMessage::SizeCheck()
{
	return static_cast<int>(DataSizeCheck(m_ID) + DataSizeCheck(m_PW));
}

inline void LoginMessage::Serialize(GameServerSerializer& serializer)
{
	// 어떤 메시지인지 판별
	// -> LoginMessage라는 Type 정보와 LoginMessage의 크기 정보를 Serialize 한다
	GameServerMessage::Serialize(serializer);

	// 이후 ID와 PW에 해당하는 string 정보를 Serialize 한다
	serializer << m_ID;
	serializer << m_PW;
}

inline void LoginMessage::Deserialize(GameServerSerializer& serializer)
{
	// serializer의 정보를 GameServerMessage의 m_Type과 m_Size에 등록
	GameServerMessage::Deserialize(serializer);

	serializer >> m_ID;
	serializer >> m_PW;
}
////////////////////////////////////////////////////////////////////////////



/////////////////////////////// QuitMessage ///////////////////////////////
inline QuitMessage::QuitMessage()
	: GameServerMessage(MessageType::Quit)
{
}

inline int QuitMessage::SizeCheck()
{
	return 0;
}

inline void QuitMessage::Serialize(GameServerSerializer& serializer)
{
	GameServerMessage::Serialize(serializer);
}

inline void QuitMessage::Deserialize(GameServerSerializer& serializer)
{
	GameServerMessage::Deserialize(serializer);
}
/////////////////////////////////////////////////////////////////////////////////////



/////////////////////////////// AttackMessage ///////////////////////////////
inline AttackMessage::AttackMessage()
	: GameServerMessage(MessageType::Attack)
{
}

inline int AttackMessage::SizeCheck()
{
	return 0;
}

inline void AttackMessage::Serialize(GameServerSerializer& serializer)
{
	GameServerMessage::Serialize(serializer);
}

inline void AttackMessage::Deserialize(GameServerSerializer& serializer)
{
	GameServerMessage::Deserialize(serializer);
}
/////////////////////////////////////////////////////////////////////////////////////