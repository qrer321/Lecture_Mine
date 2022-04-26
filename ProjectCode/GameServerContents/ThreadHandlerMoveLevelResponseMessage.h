#pragma once
#include <GameServerCore/ThreadHandlerBase.h>
#include "ClientToServer.h"

class ThreadHandlerMoveLevelResponseMessage final : public ThreadHandlerBase<MoveLevelResponseMessage>
{
private: // Member Var

public: // Default
	ThreadHandlerMoveLevelResponseMessage() = default;
	~ThreadHandlerMoveLevelResponseMessage() override = default;

	ThreadHandlerMoveLevelResponseMessage(const ThreadHandlerMoveLevelResponseMessage& other) = delete;
	ThreadHandlerMoveLevelResponseMessage(ThreadHandlerMoveLevelResponseMessage&& other) noexcept = delete;
	ThreadHandlerMoveLevelResponseMessage& operator=(const ThreadHandlerMoveLevelResponseMessage& other) = delete;
	ThreadHandlerMoveLevelResponseMessage& operator=(ThreadHandlerMoveLevelResponseMessage&& other) = delete;

public:
	void Start() override;
};

