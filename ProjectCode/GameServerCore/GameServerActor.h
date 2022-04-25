#pragma once
#include <GameServerBase/GameServerObjectBase.h>
#include <GameServerBase/GameServerNameBase.h>
#include <GameServerBase/GameServerMathStruct.h>
#include <GameServerNet/IPEndPoint.h>
#include "GameServerCore.h"

class TCPSession;
class UDPSession;
class GameServerMessage;
class GameServerSection;
class GameServerActor : public GameServerObjectBase, GameServerNameBase
{
	friend GameServerSection;

private:
	std::queue<std::shared_ptr<GameServerMessage>>	m_MessageQueue{};

	std::shared_ptr<TCPSession>						m_TCPSession;
	std::shared_ptr<UDPSession>						m_UDPSession;
	IPEndPoint										m_UDPEndPoint;

	GameServerSection*								m_Section;
	uint64_t										m_ActorIndex;
	uint64_t										m_ThreadIndex;
	uint64_t										m_SectionIndex;

	FVector4										m_ActorPos;
	FVector4										m_ActorDir;
	FVector4										m_ActorScale;

	int												m_UDPPort;

	std::atomic<bool>								m_IsSectionMove;

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

	void SetTCPSession(const std::shared_ptr<TCPSession>& session)	{ m_TCPSession = session; TCPSessionInitialize(); }
	void SetUDPSession()											{ m_UDPSession = GameServerCore::GetUDPPort(m_UDPPort); UDPSessionInitialize(); }
	void SetUDPEndPoint(const IPEndPoint& end_point)				{ m_UDPEndPoint = end_point; }
	void SetSection(GameServerSection* section)
	{
		m_Section = section;

		if (nullptr != m_Section)
		{
			SectionInitialize();
		}
	}

	virtual void Update(float delta_time) = 0;
	virtual bool InsertSection() = 0;
	virtual void DeathEvent() = 0;

	void SectionMoveEnd() { m_IsSectionMove = false; }

protected:
	std::shared_ptr<TCPSession> GetTCPSession() const { return m_TCPSession; }
	std::shared_ptr<UDPSession> GetUDPSession() const { return m_UDPSession; }
	GameServerSection*			GetSection() const { return m_Section; }

	virtual void TCPSessionInitialize() = 0;
	virtual void UDPSessionInitialize() = 0;
	virtual void SectionInitialize() = 0;

public: // Member Function
	[[nodiscard]] uint64_t	GetActorIndex() const { return m_ActorIndex; }
	[[nodiscard]] uint64_t	GetThreadIndex() const { return m_ThreadIndex; }
	[[nodiscard]] uint64_t	GetSectionIndex() const { return m_SectionIndex; }

	int						GetUDPPort() const { return m_UDPPort; }
	IPEndPoint				GetUDPEndPoint() const { return m_UDPEndPoint; }
	const IPEndPoint&		GetUDPEndPoint_Ref() const { return m_UDPEndPoint; }

	[[nodiscard]] FVector4	GetActorPos() const { return m_ActorPos; }
	[[nodiscard]] FVector4	GetActorDir() const { return m_ActorDir; }
	[[nodiscard]] FVector4	GetActorScale() const { return m_ActorScale; }

	bool GetSectionMove() const { return m_IsSectionMove;  }

	void SetActorPos(const FVector4& pos) { m_ActorPos = pos; }
	void SetActorDir(const FVector4& dir) { m_ActorDir = dir; }
	void SetActorScale(const FVector4& scale) { m_ActorScale = scale; }

	void Move(const FVector4& value) { m_ActorPos += value; }

	bool IsEmptyMessage() const { return m_MessageQueue.empty(); }
	std::shared_ptr<GameServerMessage> PopMessage();
};