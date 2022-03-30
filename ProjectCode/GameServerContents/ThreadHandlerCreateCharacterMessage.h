#pragma once
#include <GameServerCore/ThreadHandlerBase.h>
#include "ServerToClient.h"
#include "ClientToServer.h"

class ThreadHandlerCreateCharacterMessage final : public ThreadHandlerBase<CreateCharacterMessage>
{
private: // Member Var
	CreateCharacterResultMessage	m_ResultMessage;	// 처리한 결과

public: // Default
	ThreadHandlerCreateCharacterMessage() = default;
	~ThreadHandlerCreateCharacterMessage() override = default;

	ThreadHandlerCreateCharacterMessage(const ThreadHandlerCreateCharacterMessage& other) = delete;
	ThreadHandlerCreateCharacterMessage(ThreadHandlerCreateCharacterMessage&& other) noexcept = delete;
	ThreadHandlerCreateCharacterMessage& operator=(const ThreadHandlerCreateCharacterMessage& other) = delete;
	ThreadHandlerCreateCharacterMessage& operator=(ThreadHandlerCreateCharacterMessage&& other) = delete;

private:
	void DBCheck();
	void ResultSend();

public:
	void Start() override;
};

