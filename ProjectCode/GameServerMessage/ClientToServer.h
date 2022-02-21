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
	// � �޽������� �Ǻ�
	// -> LoginMessage��� Type ������ LoginMessage�� ũ�� ������ Serialize �Ѵ�
	GameServerMessage::Serialize(serializer);

	// ���� ID�� PW�� �ش��ϴ� string ������ Serialize �Ѵ�
	serializer << m_ID;
	serializer << m_PW;
}

inline void LoginMessage::Deserialize(GameServerSerializer& serializer)
{
	// serializer�� ������ GameServerMessage�� m_Type�� m_Size�� ���
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