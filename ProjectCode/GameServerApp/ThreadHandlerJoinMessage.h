#pragma once
#include <GameServerNet/TCPSession.h>
#include <GameServerMessage/Messages.h>

// �뵵 : Login ��Ŷ�� ���� ���� ó���� ThreadHandlerLoginMessage Ŭ����
class ThreadHandlerJoinMessage final : public std::enable_shared_from_this<ThreadHandlerJoinMessage>
{
private: // Member Var
	std::shared_ptr<TCPSession>		m_TCPSession;			// ��Ŷ�� ���� TCPSession
	std::shared_ptr<JoinMessage>	m_JoinMessage;			// TCPSession�� ���� JoinMessage
	JoinResultMessage				m_JoinResultMessage;	// ó���� ���

public: // Default
	ThreadHandlerJoinMessage() = delete;
	ThreadHandlerJoinMessage(std::shared_ptr<TCPSession> tcp_session, std::shared_ptr<JoinMessage> join_message);
	~ThreadHandlerJoinMessage() = default;

	ThreadHandlerJoinMessage(const ThreadHandlerJoinMessage& other) = delete;
	ThreadHandlerJoinMessage(ThreadHandlerJoinMessage&& other) noexcept = delete;

	ThreadHandlerJoinMessage& operator=(const ThreadHandlerJoinMessage& other) = delete;
	ThreadHandlerJoinMessage& operator=(ThreadHandlerJoinMessage&& other) = delete;

private:
	// �޽����� ó���ϴ� �ܰ�
	void DBCheck();

	// �޽��� ó���� ������ �� ����Ǵ� �Լ�
	void ResultSend();

public:
	// �޽����� ó�� �޾��� �� ����Ǵ� �Լ�
	void Start();
};

