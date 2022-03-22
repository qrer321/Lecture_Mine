#pragma once
#include <GameServerNet/TCPSession.h>
#include <GameServerMessage/Messages.h>

// 용도 : Login 패킷에 대한 모든걸 처리할 ThreadHandlerLoginMessage 클래스
class ThreadHandlerJoinMessage final : public std::enable_shared_from_this<ThreadHandlerJoinMessage>
{
private: // Member Var
	std::shared_ptr<TCPSession>		m_TCPSession;			// 패킷을 보낸 TCPSession
	std::shared_ptr<JoinMessage>	m_JoinMessage;			// TCPSession이 보낸 JoinMessage
	JoinResultMessage				m_JoinResultMessage;	// 처리한 결과

public: // Default
	ThreadHandlerJoinMessage() = delete;
	ThreadHandlerJoinMessage(std::shared_ptr<TCPSession> tcp_session, std::shared_ptr<JoinMessage> join_message);
	~ThreadHandlerJoinMessage() = default;

	ThreadHandlerJoinMessage(const ThreadHandlerJoinMessage& other) = delete;
	ThreadHandlerJoinMessage(ThreadHandlerJoinMessage&& other) noexcept = delete;

	ThreadHandlerJoinMessage& operator=(const ThreadHandlerJoinMessage& other) = delete;
	ThreadHandlerJoinMessage& operator=(ThreadHandlerJoinMessage&& other) = delete;

private:
	// 메시지를 처리하는 단계
	void DBCheck();

	// 메시지 처리가 끝났을 때 실행되는 함수
	void ResultSend();

public:
	// 메시지를 처음 받았을 때 실행되는 함수
	void Start();
};

