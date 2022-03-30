#pragma once

// 용도 : 
// 분류 :
// 첨언 : 
class GameServerSection
{
	friend class GameServerSectionManager;

private: // Member Var
	uint32_t m_SectionNumber;

public: // Default
	GameServerSection() = default;
	~GameServerSection() = default;

	GameServerSection(const GameServerSection& other) = delete;
	GameServerSection(GameServerSection&& other) noexcept = delete;
	GameServerSection& operator=(const GameServerSection& other) = delete;
	GameServerSection& operator=(GameServerSection&& other) = delete;

private:
	void SetSectionNumber(uint32_t section_number) { m_SectionNumber = section_number; }

public: // Member Function
	uint32_t GetSectionNumber() const { return m_SectionNumber; }

	bool Update();
};

