#pragma once
#include "Messages.h"
#include <memory>

class MessageConverter
{
private:
	const std::vector<unsigned char>&	m_Buffer;
	std::shared_ptr<GameServerMessage>	m_Message;

public: // Default
	MessageConverter() = delete;
	explicit MessageConverter(const std::vector<unsigned char>& buffer);
	~MessageConverter() = default;

	MessageConverter(const MessageConverter& other) = delete;
	MessageConverter(MessageConverter&& other) noexcept;

	MessageConverter& operator=(const MessageConverter& other) = delete;
	MessageConverter& operator=(MessageConverter&& other) = delete;

public:
	[[nodiscard]] MessageType GetMessageType() const;
	[[nodiscard]] uint32_t GetMessageType_UINT() const;
	std::shared_ptr<GameServerMessage>& GetServerMessage();

	[[nodiscard]] bool IsValid() const;
};

