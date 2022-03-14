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
 * DB���� ����, �޽��� ����, ��� ��Ŷ�� ������ ��� �ϵ���
 * ThreadHandlerLoginMessage Ŭ���� ���ο��� �̷�������ϰ�
 * �ܺη� �����Ǿ�� �ȵȴ�.
 *
 * DBQueue�� NetQueue�� GameServerQueue�� ���� �����Ѵ�
 */
ThreadHandlerLoginMessage::ThreadHandlerLoginMessage(std::shared_ptr<TCPSession> tcpSession, std::shared_ptr<LoginMessage> loginMessage)
	: m_TCPSession(std::move(tcpSession))
	, m_LoginMessage(std::move(loginMessage))
{
}

void ThreadHandlerLoginMessage::DBCheck()
{
	// GameServerThread���� thread_local�� ����
	// ������ �����尡 ���� local ������ ������⿡
	// �ڵ� ������ �ش� thread�� Name�� ���� �� �ְ� �Ǿ���.
	std::string thread_name = GameServerThread::GetName();
	// DBConnecter* db_connecter = GameServerThread::GetLocalData<DBConnecter>();
	TestClass* test_class = GameServerThread::GetLocalData<TestClass>();

	m_LoginResultMessage.m_Code = EGameServerCode::OK;

	// NetThread���� ������ ResultSend �ݹ��Լ� ���
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

	// LoginResultMessage �� LoginError�� �ʱ�ȭ
	m_LoginResultMessage.m_Code = EGameServerCode::LoginError;

	// DBThread���� ������ DBCheck �ݹ��Լ� ���
	DBQueue::Queue([self = shared_from_this()]
		{
			self->DBCheck();
		});
}
