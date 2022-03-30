#include "PreCompile.h"
#include "ThreadHandlerLoginMessage.h"
#include "UserTable.h"
#include "ContentsSystemEnum.h"
#include "ContentsUserData.h"

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
		m_ResultMessage.m_Code = EGameServerCode::LoginError;
	}
	else
	{
		m_ResultMessage.m_Code = EGameServerCode::OK;
	}

	m_RowDatum = select_query.m_RowDatum;

	// �θ� Ŭ������ ThreadHandlerBase�� ����
	// NetThread���� ������ ResultSend �ݹ��Լ� ���
	NetWork(&ThreadHandlerLoginMessage::ResultSend);
}

void ThreadHandlerLoginMessage::ResultSend()
{
	const std::shared_ptr<ContentsUserData> user_data = std::make_shared<ContentsUserData>();
	user_data->m_UserData = *m_RowDatum;
	m_TCPSession->SetLink(EDataIndex::USER_DATA, user_data);

	GameServerSerializer serializer;
	m_ResultMessage.Serialize(serializer);
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
	m_ResultMessage.m_Code = EGameServerCode::LoginError;

	// �θ� Ŭ������ ThreadHandlerBase�� ����
	// DBThread���� ������ DBCheck �ݹ��Լ� ���
	DBWork(&ThreadHandlerLoginMessage::DBCheck);
}
