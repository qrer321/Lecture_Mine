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
	float		m_LastRoomPosX;
	float		m_LastRoomPosY;
	float		m_LastRoomPosZ;

public:
	CharacterRow(
		const int index,
		std::string nickname,
		const int user_index,
		const float hp,
		const float att,
		const int last_room_id,
		const float room_x,
		const float room_y,
		const float room_z)
		: m_Index(index)
		, m_Nickname(std::move(nickname))
		, m_UserIndex(user_index)
		, m_HP(hp)
		, m_Att(att)
		, m_LastRoomID(last_room_id)
		, m_LastRoomPosX(room_x)
		, m_LastRoomPosY(room_y)
		, m_LastRoomPosZ(room_z)
	{
		
	}
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

class CharacterTable_SelectUserCharacters : public DBQuery
{
public:
	std::vector<std::shared_ptr<CharacterRow>>	m_RowData;
	int											m_UserIndex;

public: // Default
	explicit CharacterTable_SelectUserCharacters(int index);
	~CharacterTable_SelectUserCharacters() override = default;

	CharacterTable_SelectUserCharacters(const CharacterTable_SelectUserCharacters& other) = delete;
	CharacterTable_SelectUserCharacters(CharacterTable_SelectUserCharacters&& other) noexcept = delete;
	CharacterTable_SelectUserCharacters& operator=(const CharacterTable_SelectUserCharacters& other) = delete;
	CharacterTable_SelectUserCharacters& operator=(CharacterTable_SelectUserCharacters&& other) = delete;

public: // Member Function
	bool ExecuteQuery() override;
};

class CharacterTable_CreateCharacter : public DBQuery
{
public:
	std::string m_Nickname;
	int			m_Id;

public: // Default
	explicit CharacterTable_CreateCharacter(std::string nickname, int id);
	~CharacterTable_CreateCharacter() override = default;

	CharacterTable_CreateCharacter(const CharacterTable_CreateCharacter& other) = delete;
	CharacterTable_CreateCharacter(CharacterTable_CreateCharacter&& other) noexcept = delete;
	CharacterTable_CreateCharacter& operator=(const CharacterTable_CreateCharacter& other) = delete;
	CharacterTable_CreateCharacter& operator=(CharacterTable_CreateCharacter&& other) = delete;

public: // Member Function
	bool ExecuteQuery() override;
};