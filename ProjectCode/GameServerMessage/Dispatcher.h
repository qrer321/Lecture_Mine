#pragma once
#include <GameServerNet/TCPSession.h>
#include "GameServerMessage.h"

// MessageOwner -> 패킷을 받은 Session
template<class MessageOwner>
using MessageHandler = std::function<void(std::shared_ptr<MessageOwner>, std::shared_ptr<GameServerMessage>)>;

// Dispatcher의 역할
// 다양한 스레드를 넘나들고 많은 단계를 거쳐야 하는 일들을
// Dispatcher 클래스에서 확인할 수 있게 해주는 용도
template<class MessageOwner>
class Dispatcher final
{
private:
	std::unordered_map<uint32_t, MessageHandler<MessageOwner>> m_Handlers;

public: // Default
	Dispatcher() = default;
	~Dispatcher() = default;

	Dispatcher(const Dispatcher& other) = delete;
	Dispatcher(Dispatcher&& other) noexcept = delete;
	Dispatcher& operator=(const Dispatcher& other) = delete;
	Dispatcher& operator=(Dispatcher&& other) = delete;

public:
	bool GetHandler(uint32_t type, MessageHandler<MessageOwner>& handler) const;

public: // Member Function
	bool AddHandler(uint32_t type, const MessageHandler<MessageOwner>& handler);
};

template <class MessageOwner>
bool Dispatcher<MessageOwner>::GetHandler(uint32_t type, MessageHandler<MessageOwner>& handler) const
{
	auto handle = m_Handlers.find(type);
	if (handle == m_Handlers.end())
	{
		return false;
	}

	handler = handle->second;
	return true;
}

template <class MessageOwner>
bool Dispatcher<MessageOwner>::AddHandler(uint32_t type, const MessageHandler<MessageOwner>& handler)
{
	auto result = m_Handlers.insert(std::make_pair(type, handler));

	return result.second;
}