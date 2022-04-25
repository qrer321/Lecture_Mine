#pragma once
#include <GameServerCore/ThreadHandlerBase.h>
#include "ClientToServer.h"

class ThreadHandlerUDPConnectResultMessage final : public ThreadHandlerBase<UDPConnectResultMessage>
{
private: // Member Var

public: // Default
	ThreadHandlerUDPConnectResultMessage() = default;
	~ThreadHandlerUDPConnectResultMessage() override = default;

	ThreadHandlerUDPConnectResultMessage(const ThreadHandlerUDPConnectResultMessage& other) = delete;
	ThreadHandlerUDPConnectResultMessage(ThreadHandlerUDPConnectResultMessage&& other) noexcept = delete;

	ThreadHandlerUDPConnectResultMessage& operator=(const ThreadHandlerUDPConnectResultMessage& other) = delete;
	ThreadHandlerUDPConnectResultMessage& operator=(ThreadHandlerUDPConnectResultMessage&& other) = delete;

public:
	void Start() override;
};