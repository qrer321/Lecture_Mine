#pragma once
#include <GameServerBase/GameServerThread.h>
#include <GameServerMessage/GameServerMessage.h>
#include <GameServerNet/IPEndPoint.h>
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
	~GameServerSectionManager();

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

	void Init(int thread_count, const std::string& thread_name);

	template <typename SectionType, typename EnumType, typename... Parameter>
	std::shared_ptr<SectionType> CreateSection(int thread_index, EnumType enum_type, Parameter... args)
	{
		return CreateSection<SectionType>(thread_index, static_cast<uint64_t>(enum_type), args...);
	}

	template <typename SectionType, typename... Parameter>
	std::shared_ptr<SectionType> CreateSection(int thread_index, uint64_t section_index, Parameter... args)
	{
		std::shared_ptr<SectionType> new_section = std::make_shared<SectionType>(args...);
		CreateSection(thread_index, section_index, new_section);

		return new_section;
	}

	void CreateSection(int thread_index, uint64_t section_index, const std::shared_ptr<GameServerSection>& section);
	uint64_t RemoveSection(int thread_index, uint64_t section_index);

	std::shared_ptr<GameServerSection> FindSection(uint64_t section_key);

	void MessagePost(uint64_t thread_index, const std::function<void()>& callback);
	void ActorPost(uint64_t thread_index, uint64_t section_index, uint64_t actor_index, const std::shared_ptr<GameServerMessage>& message);
	void ActorEndPointPost(uint64_t thread_index, uint64_t section_index, uint64_t actor_index, const IPEndPoint& end_point, const std::shared_ptr<GameServerMessage>& message);
};

