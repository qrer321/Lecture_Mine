#pragma once
#include <GameServerCore/ThreadHandlerBase.h>
#include "ClientToServer.h"

class ThreadHandlerClientToReadyMessage final : public ThreadHandlerBase<ClientToReadyMessage>
{
private: // Member Var
	ClientToReadyMessage m_ResultMessage;	// ó���� ���

public: // Default
	ThreadHandlerClientToReadyMessage() = default;
	~ThreadHandlerClientToReadyMessage() override = default;

	ThreadHandlerClientToReadyMessage(const ThreadHandlerClientToReadyMessage& other) = delete;
	ThreadHandlerClientToReadyMessage(ThreadHandlerClientToReadyMessage&& other) noexcept = delete;
	ThreadHandlerClientToReadyMessage& operator=(const ThreadHandlerClientToReadyMessage& other) = delete;
	ThreadHandlerClientToReadyMessage& operator=(ThreadHandlerClientToReadyMessage&& other) = delete;

public:
	void Start() override;
};

