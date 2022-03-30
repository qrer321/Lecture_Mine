#pragma once
#include <GameServerCore/ThreadHandlerBase.h>
#include "ServerToClient.h"
#include "ClientToServer.h"

// �뵵 : Login ��Ŷ�� ���� ���� ó���� ThreadHandlerLoginMessage Ŭ����
class UserRow;
class ThreadHandlerLoginMessage final : public ThreadHandlerBase<LoginMessage>
{
private: // Member Var
	LoginResultMessage				m_ResultMessage;	// ó���� ���

public:
	std::shared_ptr<UserRow> m_RowDatum;

public: // Default
	ThreadHandlerLoginMessage() = default;
	~ThreadHandlerLoginMessage() override = default;

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
	void Start() override;
};

