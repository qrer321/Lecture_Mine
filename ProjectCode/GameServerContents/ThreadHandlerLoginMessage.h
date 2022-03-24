#pragma once
#include <GameServerCore/ThreadHandlerBase.h>

// 용도 : Login 패킷에 대한 모든걸 처리할 ThreadHandlerLoginMessage 클래스
class ThreadHandlerLoginMessage final : public ThreadHandlerBase<LoginMessage>
{
private: // Member Var
	LoginResultMessage				m_LoginResultMessage;	// 처리한 결과

public: // Default
	ThreadHandlerLoginMessage() = default;
	~ThreadHandlerLoginMessage() override = default;

	ThreadHandlerLoginMessage(const ThreadHandlerLoginMessage& other) = delete;
	ThreadHandlerLoginMessage(ThreadHandlerLoginMessage&& other) noexcept = delete;

	ThreadHandlerLoginMessage& operator=(const ThreadHandlerLoginMessage& other) = delete;
	ThreadHandlerLoginMessage& operator=(ThreadHandlerLoginMessage&& other) = delete;

private:
	// 메시지를 처리하는 단계
	void DBCheck();

	// 메시지 처리가 끝났을 때 실행되는 함수
	void ResultSend();

public:
	// 메시지를 처음 받았을 때 실행되는 함수
	void Start() override;
};

