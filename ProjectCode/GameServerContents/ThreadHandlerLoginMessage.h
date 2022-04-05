#pragma once
#include <GameServerCore/ThreadHandlerBase.h>
#include "ContentsUserData.h"
#include "ServerToClient.h"
#include "ClientToServer.h"

// �뵵 : Login ��Ŷ�� ���� ���� ó���� ThreadHandlerLoginMessage Ŭ����
class UserRow;
class ThreadHandlerLoginMessage final : public ThreadHandlerBase<LoginMessage>
{
private: // Member Var
	LoginResultMessage					m_ResultMessage;
	CharacterListMessage				m_CharacterListMessage;
	std::shared_ptr<ContentsUserData>	m_UserData;

public:
	std::shared_ptr<UserRow>	m_RowDatum;
	int							m_UserIndex{};

public: // Default
	ThreadHandlerLoginMessage() = default;
	~ThreadHandlerLoginMessage() override = default;

	ThreadHandlerLoginMessage(const ThreadHandlerLoginMessage& other) = delete;
	ThreadHandlerLoginMessage(ThreadHandlerLoginMessage&& other) noexcept = delete;

	ThreadHandlerLoginMessage& operator=(const ThreadHandlerLoginMessage& other) = delete;
	ThreadHandlerLoginMessage& operator=(ThreadHandlerLoginMessage&& other) = delete;

private:
	void DBCheck();
	void ResultSend();
	void DBCharacterListCheck();
	void CharactersSend();

public:
	// �޽����� ó�� �޾��� �� ����Ǵ� �Լ�
	void Start() override;
};

