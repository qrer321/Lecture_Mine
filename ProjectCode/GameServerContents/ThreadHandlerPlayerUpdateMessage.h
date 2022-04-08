#pragma once
#include <GameServerCore/ThreadHandlerBase.h>
#include "ServerAndClient.h"

class ThreadHandlerPlayerUpdateMessage final : public ThreadHandlerBase<PlayerUpdateMessage>
{
private: // Member Var
	PlayerUpdateMessage	m_ResultMessage;	// 처리한 결과

public: // Default
	ThreadHandlerPlayerUpdateMessage() = default;
	~ThreadHandlerPlayerUpdateMessage() override = default;

	ThreadHandlerPlayerUpdateMessage(const ThreadHandlerPlayerUpdateMessage& other) = delete;
	ThreadHandlerPlayerUpdateMessage(ThreadHandlerPlayerUpdateMessage&& other) noexcept = delete;
	ThreadHandlerPlayerUpdateMessage& operator=(const ThreadHandlerPlayerUpdateMessage& other) = delete;
	ThreadHandlerPlayerUpdateMessage& operator=(ThreadHandlerPlayerUpdateMessage&& other) = delete;

public:
	void Start() override;
};

