#pragma once
#include <GameServerBase/GameServerObjectBase.h>
#include <GameServerBase/GameServerUnique.h>

class TCPSession;
class GameServerActor;
class GameServerSection : public GameServerObjectBase
{
	friend class GameServerSectionManager;

private: // Member Var
	std::map<uint64_t, std::shared_ptr<GameServerActor>>	m_AllActor;
	std::list<std::shared_ptr<GameServerActor>>				m_PlayableActor;
	std::list<std::shared_ptr<GameServerActor>>				m_AIActor;

	std::vector<std::shared_ptr<GameServerActor>>			m_WaitActor;
	std::atomic<size_t>										m_WaitActorCount;
	std::mutex												m_WaitLock;

	uint64_t m_SectionKey{};
	uint64_t m_ThreadKey{};

public: // Default
	GameServerSection();
	~GameServerSection() override = default;

	GameServerSection(const GameServerSection& other) = delete;
	GameServerSection(GameServerSection&& other) noexcept = delete;
	GameServerSection& operator=(const GameServerSection& other) = delete;
	GameServerSection& operator=(GameServerSection&& other) = delete;

private:
	virtual void UserUpdate() = 0;

	void SetSectionKey(uint64_t section_key) { m_SectionKey = section_key; }
	void SetThreadKey(uint64_t thread_key) { m_ThreadKey = thread_key; }

	void InsertActor(uint64_t id, const std::shared_ptr<GameServerActor>& actor);

public:
	template <typename ActorType, typename... Parameter>
	uint64_t CreateActor(Parameter... args)
	{
		int unique_id = GameServerUnique::GetNextUniqueId();
		CreateActor<ActorType>(unique_id, args...);

		return unique_id;
	}

	template <typename ActorType, typename... Parameter>
	void CreateActor(uint64_t id, Parameter... args)
	{
		std::shared_ptr<ActorType> new_actor = std::make_shared<ActorType>(args...);
		new_actor->SetID(id);

		InsertActor(id, std::dynamic_pointer_cast<GameServerActor>(new_actor));
	}


	template <typename ActorType, typename... Parameter>
	uint64_t CreateActor(const std::shared_ptr<TCPSession>& session, Parameter... args)
	{
		uint64_t unique_id = GameServerUnique::GetNextUniqueId();
		CreateActor<ActorType>(session, unique_id, args...);

		return unique_id;
	}

	template <typename ActorType, typename... Parameter>
	void CreateActor(const std::shared_ptr<TCPSession>& session, uint64_t id, Parameter... args)
	{
		std::shared_ptr<ActorType> new_actor = std::make_shared<ActorType>(args...);
		new_actor->SetID(id);
		new_actor->SetSession(session);

		InsertActor(id, std::dynamic_pointer_cast<GameServerActor>(new_actor));
	}

public: // Member Function
	uint64_t GetSectionKey() const { return m_SectionKey; }

	bool Update(float delta_time);
	void Broadcasting();
};

