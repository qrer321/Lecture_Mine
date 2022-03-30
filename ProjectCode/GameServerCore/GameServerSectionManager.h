#pragma once
#include <GameServerBase/GameServerThread.h>
#include "GameServerSection.h"

// 용도 : 
// 분류 :
// 첨언 : 
class GameServerSectionManager
{
private: // Member Var
	static GameServerSectionManager* m_Inst;

	int m_NextSectionIndex;

	std::mutex m_SectionLock;
	std::unordered_map<uint32_t, std::shared_ptr<GameServerSection>> m_AllSection;

public: // Default
	GameServerSectionManager();
	~GameServerSectionManager() = default;

	GameServerSectionManager(const GameServerSectionManager& other) = delete;
	GameServerSectionManager(GameServerSectionManager&& other) noexcept = delete;

	GameServerSectionManager& operator=(const GameServerSectionManager& other) = delete;
	GameServerSectionManager& operator=(GameServerSectionManager&& other) = delete;

private:

public: // Member Function
	static GameServerSectionManager* GetInst() { return m_Inst; }

	static void Destroy()
	{
		if (nullptr != m_Inst)
		{
			delete m_Inst;
			m_Inst = nullptr;
		}
	}

	template <typename SectionType, typename... Parameter>
	uint32_t AddSection(Parameter... args)
	{
		return AddSection(std::make_shared<SectionType>(args...));
	}

	uint32_t AddSection(GameServerSection* section);
	uint32_t RemoveSection(uint32_t section_number);
};

