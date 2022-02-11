#pragma once
#include <GameServerNet/TCPSession.h>
#include <GameServerMessage/Messages.h>

// 용도 : 
// 분류 :
// 첨언 : 
class ThreadHandlerChatMessage final : public std::enable_shared_from_this<ThreadHandlerChatMessage>
{
private: // Member Var
	std::shared_ptr<TCPSession>		m_TCPSession;	// 패킷을 보낸 TCPSession
	std::shared_ptr<ChatMessage>	m_Message;		// TCPSession이 보낸 ChatMessage

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

