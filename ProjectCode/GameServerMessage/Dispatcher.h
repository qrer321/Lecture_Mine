#pragma once
#include <GameServerNet/TCPSession.h>
#include "Messages.h"

// MessageOwner -> ��Ŷ�� ���� Session
template<class MessageOwner>
using MessageHandler = std::function<void(std::shared_ptr<MessageOwner>, std::shared_ptr<GameServerMessage>)>;

// Dispatcher�� ����
// �پ��� �����带 �ѳ���� ���� �ܰ踦 ���ľ� �ϴ� �ϵ���
// Dispatcher Ŭ�������� Ȯ���� �� �ְ� ���ִ� �뵵
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