#pragma once
#include <GameServerNet/TCPSession.h>
#include <GameServerNet/UDPSession.h>
#include <GameServerMessage/Messages.h>
#include "GameServerSectionManager.h"
#include "DBQueue.h"
#include "NetQueue.h"

template <class MessageType, class SessionType = TCPSession>
class ThreadHandlerBase : public std::enable_shared_from_this<ThreadHandlerBase<MessageType, SessionType>>
{
protected: // Member Var
	std::shared_ptr<SessionType>	m_Session;	// 패킷을 보낸 TCPSession
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

	void Init(std::shared_ptr<SessionType> session, std::shared_ptr<MessageType> message)
	{
		m_Session = std::move(session);
		m_Message = std::move(message);
	}

	template <typename ChildThreadHandler>
	void DBWork(void(ChildThreadHandler::*child_function)())
	{
		DBQueue::Queue(std::bind(child_function, std::dynamic_pointer_cast<ChildThreadHandler>(this->shared_from_this())));
	}

	template <typename ChildThreadHandler>
	void NetWork(void(ChildThreadHandler::*child_function)())
	{
		NetQueue::Queue(std::bind(child_function, std::dynamic_pointer_cast<ChildThreadHandler>(this->shared_from_this())));
	}

	template <typename ChildThreadHandler>
	void SectionWork(uint64_t thread_index, void(ChildThreadHandler::* child_function)())
	{
		GameServerSectionManager::GetInst()->MessagePost(thread_index, std::bind(child_function, std::dynamic_pointer_cast<ChildThreadHandler>(this->shared_from_this())));
	}

	void ActorWork(uint64_t thread_index, uint64_t section_index, uint64_t actor_index, const std::shared_ptr<GameServerMessage>& message)
	{
		GameServerSectionManager::GetInst()->ActorPost(thread_index, section_index, actor_index, message);
	}
};

