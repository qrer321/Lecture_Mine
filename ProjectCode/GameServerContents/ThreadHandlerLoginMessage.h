#pragma once
#include <GameServerCore/ThreadHandlerBase.h>
#include "ContentsUserData.h"
#include "ServerToClient.h"
#include "ClientToServer.h"

// 용도 : Login 패킷에 대한 모든걸 처리할 ThreadHandlerLoginMessage 클래스
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
	// 메시지를 처음 받았을 때 실행되는 함수
	void Start() override;
};

