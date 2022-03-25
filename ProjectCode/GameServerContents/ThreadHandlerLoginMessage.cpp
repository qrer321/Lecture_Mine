#include "PreCompile.h"
#include "ThreadHandlerLoginMessage.h"
#include "UserTable.h"

/*
 * DB���� ����, �޽��� ����, ��� ��Ŷ�� ������ ��� �ϵ���
 * ThreadHandlerLoginMessage Ŭ���� ���ο��� �̷�������ϰ�
 * �ܺη� �����Ǿ�� �ȵȴ�.
 *
 * DBQueue�� NetQueue�� GameServerQueue�� ���� �����Ѵ�
 */
void ThreadHandlerLoginMessage::DBCheck()
{
	// GameServerThread���� thread_local�� ����
	// ������ �����尡 ���� local ������ ������⿡
	// �ڵ� ������ �ش� thread�� Name�� ���� �� �ְ� �Ǿ���.
	std::string thread_name = GameServerThread::GetName();

	UserTable_SelectIDFromUserInfo select_query(m_Message->m_ID);
	select_query.ExecuteQuery();

	if (nullptr == select_query.m_RowDatum)
	{
		m_LoginResultMessage.m_Code = EGameServerCode::LoginError;
	}
	else
	{
		m_LoginResultMessage.m_Code = EGameServerCode::OK;
	}

	// �θ� Ŭ������ ThreadHandlerBase�� ����
	// NetThread���� ������ ResultSend �ݹ��Լ� ���
	NetWork(&ThreadHandlerLoginMessage::ResultSend);
}

void ThreadHandlerLoginMessage::ResultSend()
{
	/*std::shared_ptr<GameServerUser> new_user = std::make_shared<GameServerUser>();
	GameServerString::UTF8ToAnsi(m_LoginMessage->m_ID, new_user->m_ID);

	m_TCPSession->SetLink(std::move(new_user));*/

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

	// LoginResultMessage �� LoginError�� �ʱ�ȭ
	m_LoginResultMessage.m_Code = EGameServerCode::LoginError;

	// �θ� Ŭ������ ThreadHandlerBase�� ����
	// DBThread���� ������ DBCheck �ݹ��Լ� ���
	DBWork(&ThreadHandlerLoginMessage::DBCheck);
}
