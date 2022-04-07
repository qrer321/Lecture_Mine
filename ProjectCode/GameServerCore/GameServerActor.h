#pragma once
#include <GameServerBase/GameServerObjectBase.h>
#include <GameServerBase/GameServerNameBase.h>

class TCPSession;
class GameServerSection;
class GameServerActor : public GameServerObjectBase, GameServerNameBase
{
	friend GameServerSection;

private:
	std::shared_ptr<TCPSession> m_TCPSession;
	uint64_t					m_ID;

	GameServerSection*			m_Section;

public: // Default
	GameServerActor();
	virtual ~GameServerActor() override = default;

	GameServerActor(const GameServerActor& other) = delete;
	GameServerActor(GameServerActor&& other) noexcept = delete;

	GameServerActor& operator=(const GameServerActor& other) = delete;
	GameServerActor& operator=(GameServerActor&& other) = delete;

private:
	void SetID(uint64_t id) { m_ID = id; }
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
	[[nodiscard]] uint64_t GetID() const { return m_ID; }
};