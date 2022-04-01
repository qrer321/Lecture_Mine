#pragma once
#include <GameServerBase/GameServerSerializer.h>

struct CharacterInfo 
{
public:
	int			m_Index;
	std::string m_Nickname;
	int			m_UserIndex;
	float		m_HP;
	float		m_Att;
	int			m_LastRoomID;
	float		m_LastRoomPosX;
	float		m_LastRoomPosY;
	float		m_LastRoomPosZ;

public:
	[[nodiscard]] int GetDataSize() const { return (4 * 8) + 4 + static_cast<int>(m_Nickname.size()); }

	void Serialize(GameServerSerializer& serializer)
	{
		serializer << m_Index;
		serializer << m_Nickname;
		serializer << m_UserIndex;
		serializer << m_HP;
		serializer << m_Att;
		serializer << m_LastRoomID;
		serializer << m_LastRoomPosX;
		serializer << m_LastRoomPosY;
		serializer << m_LastRoomPosZ;
	}

	void DeSerialize(GameServerSerializer& serializer)
	{
		serializer >> m_Index;
		serializer >> m_Nickname;
		serializer >> m_UserIndex;
		serializer >> m_HP;
		serializer >> m_Att;
		serializer >> m_LastRoomID;
		serializer >> m_LastRoomPosX;
		serializer >> m_LastRoomPosY;
		serializer >> m_LastRoomPosZ;
	}
};