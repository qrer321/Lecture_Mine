#pragma once
#include <GameServerBase/GameServerSerializer.h>

struct FCharacterInfo 
{
public:
	int			m_Index{};
	std::string m_Nickname;
	int			m_UserIndex{};
	float		m_HP{};
	float		m_Att{};
	int			m_LastSectionID{};
	float		m_LastSectionPosX{};
	float		m_LastSectionPosY{};
	float		m_LastSectionPosZ{};

public:
	[[nodiscard]] int GetDataSize() const { return (4 * 8) + 4 + static_cast<int>(m_Nickname.size()); }

	void Serialize(GameServerSerializer& serializer)
	{
		serializer << m_Index;
		serializer << m_Nickname;
		serializer << m_UserIndex;
		serializer << m_HP;
		serializer << m_Att;
		serializer << m_LastSectionID;
		serializer << m_LastSectionPosX;
		serializer << m_LastSectionPosY;
		serializer << m_LastSectionPosZ;
	}

	void DeSerialize(GameServerSerializer& serializer)
	{
		serializer >> m_Index;
		serializer >> m_Nickname;
		serializer >> m_UserIndex;
		serializer >> m_HP;
		serializer >> m_Att;
		serializer >> m_LastSectionID;
		serializer >> m_LastSectionPosX;
		serializer >> m_LastSectionPosY;
		serializer >> m_LastSectionPosZ;
	}

public:
	FCharacterInfo() = default;
	~FCharacterInfo() = default;
};