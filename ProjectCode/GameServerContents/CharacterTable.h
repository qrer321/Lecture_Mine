#pragma once
#include <GameServerNet/DBQuery.h>
#include "ContentsStructure.h"

class CharacterRow : std::enable_shared_from_this<CharacterRow>
{
public:
	union
	{
		struct
		{
			int			m_Index;
			std::string m_Nickname;
			int			m_UserIndex;
			float		m_HP;
			float		m_Att;
			int			m_LastSectionID;
			float		m_LastSectionPosX;
			float		m_LastSectionPosY;
			float		m_LastSectionPosZ;
		};
		FCharacterInfo	m_Info {};
	};

	CharacterRow(
		const int	index,
		std::string nickname,
		const int	user_index,
		const float hp,
		const float att,
		const int	last_section_id,
		const float last_section_x,
		const float last_section_y,
		const float last_section_z)
		: m_Index(index)
		, m_Nickname(std::move(nickname))
		, m_UserIndex(user_index)
		, m_HP(hp)
		, m_Att(att)
		, m_LastSectionID(last_section_id)
		, m_LastSectionPosX(last_section_x)
		, m_LastSectionPosY(last_section_y)
		, m_LastSectionPosZ(last_section_z)
	{}

	CharacterRow(const CharacterRow& other)
		: m_Index(other.m_Index)
		, m_Nickname(other.m_Nickname)
		, m_UserIndex(other.m_UserIndex)
		, m_HP(other.m_HP)
		, m_Att(other.m_Att)
		, m_LastSectionID(other.m_LastSectionID)
		, m_LastSectionPosX(other.m_LastSectionPosX)
		, m_LastSectionPosY(other.m_LastSectionPosY)
		, m_LastSectionPosZ(other.m_LastSectionPosZ)
	{}

	CharacterRow(const FCharacterInfo& other)
		: m_Info(other)
	{}

	CharacterRow(CharacterRow&& other) noexcept = default;
	CharacterRow& operator=(const CharacterRow& other) = delete;
	CharacterRow& operator=(CharacterRow&& other) = delete;

	~CharacterRow() {}
};

class CharacterTable_SelectNickname : public DBQuery
{
public:
	std::string						m_Nickname;
	std::shared_ptr<CharacterRow>	m_RowDatum;

public: // Default
	explicit CharacterTable_SelectNickname(std::string nickname);
	~CharacterTable_SelectNickname() override = default;

	CharacterTable_SelectNickname(const CharacterTable_SelectNickname& other) = delete;
	CharacterTable_SelectNickname(CharacterTable_SelectNickname&& other) noexcept = delete;
	CharacterTable_SelectNickname& operator=(const CharacterTable_SelectNickname& other) = delete;
	CharacterTable_SelectNickname& operator=(CharacterTable_SelectNickname&& other) = delete;

public: // Member Function
	bool ExecuteQuery() override;

	FCharacterInfo ConvertInfoCharacter() const;
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