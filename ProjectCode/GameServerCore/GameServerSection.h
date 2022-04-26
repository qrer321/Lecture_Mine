#pragma once
#include <GameServerBase/GameServerObjectBase.h>
#include <GameServerBase/GameServerNameBase.h>
#include <GameServerBase/GameServerUnique.h>
#include <GameServerNet/IPEndPoint.h>

class TCPSession;
class GameServerMessage;
class GameServerSectionManager;
class GameServerSectionThread;
class GameServerActor;
class GameServerCollision;
class GameServerSection : public GameServerObjectBase, public GameServerNameBase
{
	friend GameServerSectionManager;
	friend GameServerSectionThread;
	friend GameServerCollision;

private:
	class MoveActorData
	{
	public:
		std::shared_ptr<GameServerActor>	m_MoveActor;
		GameServerSection*					m_NextSection;
	};

private: // Member Var
	std::map<uint64_t, std::shared_ptr<GameServerActor>>	m_AllActor;
	std::list<std::shared_ptr<GameServerActor>>				m_PlayableActor;
	std::list<std::shared_ptr<GameServerActor>>				m_AIActor;

	std::vector<std::shared_ptr<GameServerActor>>			m_WaitActor;
	std::atomic<size_t>										m_WaitActorCount;
	std::mutex												m_WaitLock;

	GameServerSectionThread*								m_SectionThread;

	uint64_t m_SectionIndex{};
	uint64_t m_ThreadIndex{};

	std::map<int, std::list<GameServerCollision*>> m_CollisionList;
	std::vector<MoveActorData> m_MoveActors;

public: // Default
	GameServerSection();
	~GameServerSection() override = default;

	GameServerSection(const GameServerSection& other) = delete;
	GameServerSection(GameServerSection&& other) noexcept = delete;
	GameServerSection& operator=(const GameServerSection& other) = delete;
	GameServerSection& operator=(GameServerSection&& other) = delete;

public:
	template <typename ActorType, typename... Parameter>
	std::shared_ptr<ActorType> CreateActor(Parameter... args)
	{
		uint64_t actor_index = GameServerUnique::GetNextUniqueId();

		std::shared_ptr<ActorType> new_actor = CreateSessionActor<ActorType>(actor_index, args...);
		return new_actor;
	}

	template <typename ActorType, typename... Parameter>
	std::shared_ptr<ActorType> CreateActor(uint64_t actor_index, Parameter... args)
	{
		std::shared_ptr<ActorType> new_actor = std::make_shared<ActorType>(args...);
		new_actor->SetActorIndex(actor_index);

		InsertActor(std::dynamic_pointer_cast<GameServerActor>(new_actor));

		return new_actor;
	}


	template <typename ActorType, typename... Parameter>
	std::shared_ptr<ActorType> CreateSessionActor(const std::shared_ptr<TCPSession>& session, Parameter... args)
	{
		uint64_t actor_index = GameServerUnique::GetNextUniqueId();

		std::shared_ptr<ActorType> new_actor = CreateSessionActor<ActorType>(session, actor_index, args...);
		return new_actor;
	}

	template <typename ActorType, typename... Parameter>
	std::shared_ptr<ActorType> CreateSessionActor(const std::shared_ptr<TCPSession>& session, uint64_t actor_index, Parameter... args)
	{
		std::shared_ptr<ActorType> new_actor = std::make_shared<ActorType>(args...);
		new_actor->SetActorIndex(actor_index);
		new_actor->SetTCPSession(session);
		new_actor->SetUDPSession();

		InsertActor(actor_index, std::dynamic_pointer_cast<GameServerActor>(new_actor));

		return new_actor;
	}

	template <typename CollisionType>
	GameServerCollision* CreateCollision(CollisionType collision_type, GameServerActor* owner_actor)
	{
		return CreateCollision(static_cast<int>(collision_type), owner_actor);
	}

	GameServerCollision* CreateCollision(int collision_type, GameServerActor* owner_actor);

private:
	virtual void UserUpdate() = 0;

	void SetSectionThread(GameServerSectionThread* section_thread) { m_SectionThread = section_thread; }

	void SetSectionIndex(uint64_t section_index) { m_SectionIndex = section_index; }
	void SetThreadIndex(uint64_t thread_index) { m_ThreadIndex = thread_index; }

	void InsertActor(uint64_t id, const std::shared_ptr<GameServerActor>& actor);

public: // Member Function
	GameServerSectionThread* GetSectionThread() const { return m_SectionThread; }

	uint64_t GetSectionIndex() const { return m_SectionIndex; }
	uint64_t GetThreadIndex() const { return m_ThreadIndex; }

	const std::list<std::shared_ptr<GameServerActor>>& GetPlayableActor() { return m_PlayableActor; }

	virtual bool Init() = 0;
	bool Update(float delta_time);
	void Release();

	void Broadcasting_TCP(const std::vector<unsigned char>& buffer, uint64_t ignore_actor = -1);
	void Broadcasting_UDP(const std::vector<unsigned char>& buffer, uint64_t ignore_actor = -1);

	void ActorPost(uint64_t actor_index, const std::shared_ptr<GameServerMessage>& message);
	void ActorEndPointPost(uint64_t actor_index, const IPEndPoint& end_point, const std::shared_ptr<GameServerMessage>& message);

	void MoveActor(const std::shared_ptr<GameServerActor>& actor);
	void MoveSection(const std::shared_ptr<GameServerActor>& move_actor, GameServerSection* dest_section);
};

