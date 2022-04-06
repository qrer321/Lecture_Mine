#pragma once
#include <GameServerBase/GameServerObjectBase.h>

class TCPSession;
class GameServerSection;
class GameServerActor : public GameServerObjectBase
{
	friend GameServerSection;

private:
	std::shared_ptr<TCPSession> m_TCPSession;
	uint64_t					m_ID;

public: // Default
	GameServerActor();
	virtual ~GameServerActor() override = default;

	GameServerActor(const GameServerActor& other) = delete;
	GameServerActor(GameServerActor&& other) noexcept = delete;

	GameServerActor& operator=(const GameServerActor& other) = delete;
	GameServerActor& operator=(GameServerActor&& other) = delete;

private:
	void SetID(uint64_t id) { m_ID = id; }
	void SetSession(const std::shared_ptr<TCPSession>& session) { m_TCPSession = session; }

	virtual void Update() = 0;

public: // Member Function
	[[nodiscard]] uint64_t GetID() const { return m_ID; }
};