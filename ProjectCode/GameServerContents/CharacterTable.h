#pragma once
#include <GameServerNet/DBQuery.h>

class CharacterRow : std::enable_shared_from_this<CharacterRow>
{
public:
	int			m_Index;
	std::string m_Nickname;
	int			m_UserIndex;
	float		m_HP;
	float		m_Att;
	int			m_LastRoomID;
	float		m_RoomPosX;
	float		m_RoomPosY;
	float		m_RoomPosZ;
};

class CharacterTable_SelectNickname : public DBQuery
{
public:
	std::string m_Nickname;

public: // Default
	explicit CharacterTable_SelectNickname(std::string nickname);
	~CharacterTable_SelectNickname() override = default;

	CharacterTable_SelectNickname(const CharacterTable_SelectNickname& other) = delete;
	CharacterTable_SelectNickname(CharacterTable_SelectNickname&& other) noexcept = delete;

	CharacterTable_SelectNickname& operator=(const CharacterTable_SelectNickname& other) = delete;
	CharacterTable_SelectNickname& operator=(CharacterTable_SelectNickname&& other) = delete;

private:

public: // Member Function
	bool ExecuteQuery() override;
};

