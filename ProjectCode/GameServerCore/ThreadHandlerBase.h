#pragma once
#include <GameServerNet/TCPSession.h>
#include <GameServerMessage/Messages.h>
#include "DBQueue.h"
#include "NetQueue.h"

template <class MessageType>
class ThreadHandlerBase : public std::enable_shared_from_this<ThreadHandlerBase<MessageType>>
{
protected: // Member Var
	std::shared_ptr<TCPSession>		m_TCPSession;	// 패킷을 보낸 TCPSession
	std::shared_ptr<MessageType>	m_Message;		// TCPSession이 보낸 MessageType

public: // Default
	ThreadHandlerBase() = default;
	virtual ~ThreadHandlerBase() = default;

	ThreadHandlerBase(const ThreadHandlerBase& other) = delete;
	ThreadHandlerBase(ThreadHandlerBase&& other) noexcept = delete;

	ThreadHandlerBase& operator=(const ThreadHandlerBase& other) = delete;
	ThreadHandlerBase& operator=(ThreadHandlerBase&& other) = delete;

public:
	virtual void Start() = 0;

	void Init(std::shared_ptr<TCPSession> tcp_session, std::shared_ptr<MessageType> message)
	{
		m_TCPSession = std::move(tcp_session);
		m_Message = std::move(message);
	}

	template<typename ChildThreadHandler>
	void DBWork(void(ChildThreadHandler::* child_function)())
	{
		DBQueue::Queue(std::bind(child_function, std::dynamic_pointer_cast<ChildThreadHandler>(this->shared_from_this())));
	}

	template<typename ChildThreadHandler>
	void NetWork(void(ChildThreadHandler::* child_function)())
	{
		NetQueue::Queue(std::bind(child_function, std::dynamic_pointer_cast<ChildThreadHandler>(this->shared_from_this())));
	}
};

