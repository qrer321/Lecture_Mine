#include "PreCompile.h"
#include "MessageConverter.h"

MessageConverter::MessageConverter(const std::vector<unsigned char>& buffer)
	: m_Buffer(buffer)
{
	GameServerSerializer serializer = GameServerSerializer(buffer);
	MessageType id;

	memcpy_s(&id, sizeof(MessageType), &buffer[0], sizeof(MessageType));
	switch (id)
	{
		// Client To Server
	case MessageType::Login:
		m_Message = std::make_shared<LoginMessage>();
		break;
	case MessageType::Quit:
		break;

		// Server To Client And Client To Server
	case MessageType::Chat:
		m_Message = std::make_shared<ChatMessage>();
		break;
	case MessageType::Attack:
		break;

		// Server To Client
	case MessageType::LoginResult:
		m_Message = std::make_shared<LoginResultMessage>();
		break;
	case MessageType::ServerDestroy:
		m_Message = std::make_shared<ServerDestroyMessage>();
		break;

	case MessageType::MAX:
		return;
	default:
		return;
	}

	if (nullptr == m_Message)
	{
		return;
	}

	m_Message->Deserialize(serializer);
}

MessageConverter::MessageConverter(MessageConverter&& other) noexcept
	: m_Buffer(other.m_Buffer)
	, m_Message(std::move(other.m_Message))
{
}

MessageType MessageConverter::GetMessageType() const
{
	return m_Message->GetType();
}

uint32_t MessageConverter::GetMessageType_UINT() const
{
	return static_cast<uint32_t>(GetMessageType());
}

std::shared_ptr<GameServerMessage>& MessageConverter::GetServerMessage()
{
	return m_Message;
}

bool MessageConverter::IsValid() const
{
	return nullptr != m_Message;
}
