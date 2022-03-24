#pragma once

class CharacterRow : std::enable_shared_from_this<CharacterRow>
{
public:
	int			m_Index;
	std::string m_ID;
	float		m_HP;
	float		m_Att;
};

class CharacterTable
{
private: // Member Var

public: // Default
	CharacterTable() = default;
	~CharacterTable() = default;

	CharacterTable(const CharacterTable& other) = delete;
	CharacterTable(CharacterTable&& other) noexcept = delete;

	CharacterTable& operator=(const CharacterTable& other) = delete;
	CharacterTable& operator=(CharacterTable&& other) = delete;

private:

public: // Member Function
};

