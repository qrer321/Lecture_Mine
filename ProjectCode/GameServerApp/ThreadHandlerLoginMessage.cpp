#include "PreCompile.h"
#include "ThreadHandlerLoginMessage.h"
#include <GameServerBase/GameServerString.h>
#include <GameServerBase/GameServerDebug.h>
#include "GameServerUser.h"
#include "DBQueue.h"
#include "NetQueue.h"

class DBConnecter
{
	
};

/*
 * DB로의 접속, 메시지 검증, 결과 패킷을 보내는 모든 일들은
 * ThreadHandlerLoginMessage 클래스 내부에서 이루어져야하고
 * 외부로 공개되어서는 안된다.
 *
 * DBQueue와 NetQueue는 GameServerQueue를 통해 동작한다
 */
ThreadHandlerLoginMessage::ThreadHandlerLoginMessage(std::shared_ptr<TCPSession> tcpSession, std::shared_ptr<LoginMessage> loginMessage)
	: m_TCPSession(std::move(tcpSession))
	, m_LoginMessage(std::move(loginMessage))
{
}

void ThreadHandlerLoginMessage::DBCheck()
{
	// GameServerThread에서 thread_local을 통해
	// 각각의 스레드가 가질 local 변수를 만들었기에
	// 코드 상으로 해당 thread의 Name을 얻어올 수 있게 되었다.
	std::string thread_name = GameServerThread::GetName();
	// DBConnecter* db_connecter = GameServerThread::GetLocalData<DBConnecter>();
	TestClass* test_class = GameServerThread::GetLocalData<TestClass>();

	m_LoginResultMessage.m_Code = EGameServerCode::OK;

	// NetThread에서 동작할 ResultSend 콜백함수 등록
	NetQueue::Queue([self = shared_from_this()]
		{
			self->ResultSend();
		});
}

void ThreadHandlerLoginMessage::ResultSend()
{
	std::shared_ptr<GameServerUser> new_user = std::make_shared<GameServerUser>();
	GameServerString::UTF8ToAnsi(m_LoginMessage->m_ID, new_user->m_ID);

	m_TCPSession->SetLink(std::move(new_user));

	GameServerSerializer serializer;
	m_LoginResultMessage.Serialize(serializer);
	m_TCPSession->Send(serializer.GetData());
}

void ThreadHandlerLoginMessage::Start()
{
	if (nullptr == m_TCPSession)
	{
		GameServerDebug::LogError("Login TCPSession Error");
		return;
	}

	std::string thread_name = GameServerThread::GetName();

	// LoginResultMessage 값 LoginError로 초기화
	m_LoginResultMessage.m_Code = EGameServerCode::LoginError;

	// DBThread에서 동작할 DBCheck 콜백함수 등록
	DBQueue::Queue([self = shared_from_this()]
		{
			self->DBCheck();
		});
}
