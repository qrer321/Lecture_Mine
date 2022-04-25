#pragma once
#include <GameServerCore/ThreadHandlerBase.h>
#include "ServerAndClient.h"

class ThreadHandlerPlayerUpdateMessage_UDP final : public ThreadHandlerBase<PlayerUpdateMessage, UDPSession>
{
private: // Member Var
	IPEndPoint m_EndPoint;

public: // Default
	ThreadHandlerPlayerUpdateMessage_UDP() = default;
	~ThreadHandlerPlayerUpdateMessage_UDP() override = default;

	ThreadHandlerPlayerUpdateMessage_UDP(const ThreadHandlerPlayerUpdateMessage_UDP& other) = delete;
	ThreadHandlerPlayerUpdateMessage_UDP(ThreadHandlerPlayerUpdateMessage_UDP&& other) noexcept = delete;
	ThreadHandlerPlayerUpdateMessage_UDP& operator=(const ThreadHandlerPlayerUpdateMessage_UDP& other) = delete;
	ThreadHandlerPlayerUpdateMessage_UDP& operator=(ThreadHandlerPlayerUpdateMessage_UDP&& other) = delete;

public:
	void SetEndPoint(const IPEndPoint& end_point) { m_EndPoint = end_point; }

	void Start() override;
};

