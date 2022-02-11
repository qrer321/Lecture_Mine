#pragma once
#include <GameServerNet/TCPSession.h>
#include <GameServerMessage/Messages.h>

// �뵵 : 
// �з� :
// ÷�� : 
class ThreadHandlerChatMessage final : public std::enable_shared_from_this<ThreadHandlerChatMessage>
{
private: // Member Var
	std::shared_ptr<TCPSession>		m_TCPSession;	// ��Ŷ�� ���� TCPSession
	std::shared_ptr<ChatMessage>	m_Message;		// TCPSession�� ���� ChatMessage

public: // Default
	ThreadHandlerChatMessage() = default;
	ThreadHandlerChatMessage(std::shared_ptr<TCPSession> tcp_session, std::shared_ptr<ChatMessage> message);
	~ThreadHandlerChatMessage() = default;

	ThreadHandlerChatMessage(const ThreadHandlerChatMessage& other) = delete;
	ThreadHandlerChatMessage(ThreadHandlerChatMessage&& other) noexcept = delete;

	ThreadHandlerChatMessage& operator=(const ThreadHandlerChatMessage& other) = delete;
	ThreadHandlerChatMessage& operator=(ThreadHandlerChatMessage&& other) = delete;

public:
	void Start();
};

