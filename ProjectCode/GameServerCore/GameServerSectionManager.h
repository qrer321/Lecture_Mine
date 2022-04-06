#pragma once
#include <GameServerBase/GameServerThread.h>
#include "GameServerSectionThread.h"
#include "GameServerSection.h"

class GameServerSectionManager
{
private: // Member Var
	static GameServerSectionManager* m_Inst;

	std::mutex m_SectionLock;
	std::unordered_map<uint64_t, std::shared_ptr<GameServerSection>> m_AllSection;

	std::vector<std::shared_ptr<GameServerSectionThread>> m_SectionThread;

public: // Default
	GameServerSectionManager() = default;
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

	void Init(int thread_count);

	template <typename SectionType, typename EnumType, typename... Parameter>
	void CreateSection(int thread_key, EnumType enum_type, Parameter... args)
	{
		CreateSection<SectionType>(thread_key, static_cast<uint64_t>(enum_type), args...);
	}

	template <typename SectionType, typename... Parameter>
	void CreateSection(int thread_key, uint64_t section_key, Parameter... args)
	{
		CreateSection(thread_key, section_key, std::make_shared<SectionType>(args...));
	}

	void CreateSection(int thread_key, uint64_t section_key, std::shared_ptr<GameServerSection> section);
	uint64_t RemoveSection(int thread_key, uint64_t section_key);

	std::shared_ptr<GameServerSection> FindSection(uint64_t section_key);
};

