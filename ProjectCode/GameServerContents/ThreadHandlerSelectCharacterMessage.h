#pragma once
#include <GameServerCore/ThreadHandlerBase.h>
#include "ClientToServer.h"

class ThreadHandlerSelectCharacterMessage final : public ThreadHandlerBase<SelectCharacterMessage>
{
private: // Member Var
	SelectCharacterMessage m_SelectMessage;	// 처리한 결과

public: // Default
	ThreadHandlerSelectCharacterMessage() = default;
	~ThreadHandlerSelectCharacterMessage() override = default;

	ThreadHandlerSelectCharacterMessage(const ThreadHandlerSelectCharacterMessage& other) = delete;
	ThreadHandlerSelectCharacterMessage(ThreadHandlerSelectCharacterMessage&& other) noexcept = delete;
	ThreadHandlerSelectCharacterMessage& operator=(const ThreadHandlerSelectCharacterMessage& other) = delete;
	ThreadHandlerSelectCharacterMessage& operator=(ThreadHandlerSelectCharacterMessage&& other) = delete;

private:
	void DBCheck();
	void ResultSend();

public:
	void Start() override;
};

