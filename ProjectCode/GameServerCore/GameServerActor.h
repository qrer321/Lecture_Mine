#pragma once
#include <GameServerBase/GameServerObjectBase.h>
#include <GameServerBase/GameServerNameBase.h>
#include <GameServerBase/GameServerMathStruct.h>

class TCPSession;
class GameServerSection;
class GameServerActor : public GameServerObjectBase, GameServerNameBase
{
	friend GameServerSection;

private:
	std::shared_ptr<TCPSession> m_TCPSession;

	GameServerSection*			m_Section;
	uint64_t					m_ActorIndex;
	uint64_t					m_ThreadIndex;
	uint64_t					m_SectionIndex;

	FVector						m_ActorPos;
	FVector						m_ActorDir;

public: // Default
	GameServerActor();
	virtual ~GameServerActor() override = default;

	GameServerActor(const GameServerActor& other) = delete;
	GameServerActor(GameServerActor&& other) noexcept = delete;

	GameServerActor& operator=(const GameServerActor& other) = delete;
	GameServerActor& operator=(GameServerActor&& other) = delete;

private:
	void SetActorIndex(uint64_t actor_index) { m_ActorIndex = actor_index; }
	void SetThreadIndex(uint64_t thread_index) { m_ThreadIndex = thread_index; }
	void SetSectionIndex(uint64_t section_index) { m_SectionIndex = section_index; }

	void SetSession(const std::shared_ptr<TCPSession>& session) { m_TCPSession = session; SessionInitialize(); }
	void SetSection(GameServerSection* section)					{ m_Section = section; SectionInitialize(); }

	virtual void Update(float delta_time) = 0;
	virtual bool InsertSection() = 0;

protected:
	std::shared_ptr<TCPSession> GetSession() const { return m_TCPSession; }
	GameServerSection*			GetSection() const { return m_Section; }

	virtual void SessionInitialize() = 0;
	virtual void SectionInitialize() = 0;

public: // Member Function
	[[nodiscard]] uint64_t GetActorIndex() const { return m_ActorIndex; }
	[[nodiscard]] uint64_t GetThreadIndex() const { return m_ThreadIndex; }
	[[nodiscard]] uint64_t GetSectionIndex() const { return m_SectionIndex; }

	[[nodiscard]] FVector GetActorPos() const { return m_ActorPos; }
	[[nodiscard]] FVector GetActorDir() const { return m_ActorDir; }

	void SetActorPos(const FVector& pos) { m_ActorPos = pos; }
	void SetActorDir(const FVector& dir) { m_ActorDir = dir; }

	void Move(const FVector& value) { m_ActorPos += value; }
};