#pragma once
#include <GameServerCore/ThreadHandlerBase.h>

class ThreadHandlerJoinMessage final : public ThreadHandlerBase<JoinMessage>
{
private: // Member Var
	JoinResultMessage				m_JoinResultMessage;	// 처리한 결과

public: // Default
	ThreadHandlerJoinMessage() = default;
	~ThreadHandlerJoinMessage() override = default;

	ThreadHandlerJoinMessage(const ThreadHandlerJoinMessage& other) = delete;
	ThreadHandlerJoinMessage(ThreadHandlerJoinMessage&& other) noexcept = delete;
	ThreadHandlerJoinMessage& operator=(const ThreadHandlerJoinMessage& other) = delete;
	ThreadHandlerJoinMessage& operator=(ThreadHandlerJoinMessage&& other) = delete;

private:
	void DBCheck();
	void ResultSend();

public:
	void Start() override;
};

