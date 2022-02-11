#pragma once
#include <GameServerNet/TCPSession.h>
#include <GameServerMessage/Messages.h>

// �뵵 : Login ��Ŷ�� ���� ���� ó���� ThreadHandlerLoginMessage Ŭ����
class ThreadHandlerLoginMessage final : public std::enable_shared_from_this<ThreadHandlerLoginMessage>
{
private: // Member Var
	std::shared_ptr<TCPSession>		m_TCPSession;			// ��Ŷ�� ���� TCPSession
	std::shared_ptr<LoginMessage>	m_LoginMessage;			// TCPSession�� ���� LoginMessage
	LoginResultMessage				m_LoginResultMessage;	// ó���� ���

public: // Default
	ThreadHandlerLoginMessage() = delete;
	ThreadHandlerLoginMessage(std::shared_ptr<TCPSession> tcpSession, std::shared_ptr<LoginMessage> loginMessage);
	~ThreadHandlerLoginMessage() = default;

	ThreadHandlerLoginMessage(const ThreadHandlerLoginMessage& other) = delete;
	ThreadHandlerLoginMessage(ThreadHandlerLoginMessage&& other) noexcept = delete;

	ThreadHandlerLoginMessage& operator=(const ThreadHandlerLoginMessage& other) = delete;
	ThreadHandlerLoginMessage& operator=(ThreadHandlerLoginMessage&& other) = delete;

private:
	// �޽����� ó���ϴ� �ܰ�
	void DBCheck();

	// �޽��� ó���� ������ �� ����Ǵ� �Լ�
	void ResultSend();

public:
	// �޽����� ó�� �޾��� �� ����Ǵ� �Լ�
	void Start();
};

