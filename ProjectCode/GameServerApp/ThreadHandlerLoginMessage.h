#pragma once
#include <GameServerNet/TCPSession.h>
#include <GameServerMessage/Messages.h>

// 용도 : Login 패킷에 대한 모든걸 처리할 ThreadHandlerLoginMessage 클래스
class ThreadHandlerLoginMessage final : public std::enable_shared_from_this<ThreadHandlerLoginMessage>
{
private: // Member Var
	std::shared_ptr<TCPSession>		m_TCPSession;			// 패킷을 보낸 TCPSession
	std::shared_ptr<LoginMessage>	m_LoginMessage;			// TCPSession이 보낸 LoginMessage
	LoginResultMessage				m_LoginResultMessage;	// 처리한 결과

public: // Default
	ThreadHandlerLoginMessage() = delete;
	ThreadHandlerLoginMessage(std::shared_ptr<TCPSession> tcpSession, std::shared_ptr<LoginMessage> loginMessage);
	~ThreadHandlerLoginMessage() = default;

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
	void Start();
};

