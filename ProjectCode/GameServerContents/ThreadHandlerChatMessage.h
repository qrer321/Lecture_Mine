#pragma once
#include <GameServerCore/ThreadHandlerBase.h>
#include "ServerAndClient.h"

class ThreadHandlerChatMessage final : public ThreadHandlerBase<ChatMessage>
{
public: // Default
	ThreadHandlerChatMessage() = default;
	~ThreadHandlerChatMessage() override = default;

	ThreadHandlerChatMessage(const ThreadHandlerChatMessage& other) = delete;
	ThreadHandlerChatMessage(ThreadHandlerChatMessage&& other) noexcept = delete;
	ThreadHandlerChatMessage& operator=(const ThreadHandlerChatMessage& other) = delete;
	ThreadHandlerChatMessage& operator=(ThreadHandlerChatMessage&& other) = delete;

public:
	void Start() override;
};

