#include "PreCompile.h"
#include "ThreadHandlerJoinMessage.h"
#include "UserTable.h"

void ThreadHandlerJoinMessage::DBCheck()
{
	UserTable_SelectIDFromUserInfo select_query(m_Message->m_ID);
	select_query.ExecuteQuery();

	if (nullptr != select_query.m_RowDatum)
	{
		// select_query�� ���� �̹� DB���� �ش� ID�� �����ϴ� ��� Error
		m_JoinResultMessage.m_Code = EGameServerCode::JoinError;
	}
	else
	{
		UserTable_InsertToUserInfo insert_query(m_Message->m_ID, m_Message->m_PW);
		if (false == insert_query.ExecuteQuery())
		{
			// insert_query�� ���� DB�� INSERT �Ͽ�����
			// ������ ���� Row�� ���� ��� Error
			m_JoinResultMessage.m_Code = EGameServerCode::JoinError;
		}
		else
		{
			// Insert ������ �������� �������� ��� Join OK
			m_JoinResultMessage.m_Code = EGameServerCode::OK;
		}
	}

	NetWork(&ThreadHandlerJoinMessage::ResultSend);
}

void ThreadHandlerJoinMessage::ResultSend()
{
	// ������ EGameServerCode�� Serialize�Ͽ� Session�� Send
	// Ŭ���̾�Ʈ������ �ش� ������ Deserialize�Ͽ� ���ۿ� ����Ͽ��� �Ѵ�.
	GameServerSerializer serializer;
	m_JoinResultMessage.Serialize(serializer);
	m_TCPSession->Send(serializer.GetData());
}

void ThreadHandlerJoinMessage::Start()
{
	if (nullptr == m_TCPSession)
	{
		GameServerDebug::LogError("Join TCPSession Error");
		return;
	}

	// LoginResultMessage �� JoinError�� �ʱ�ȭ
	m_JoinResultMessage.m_Code = EGameServerCode::JoinError;

	// �θ� Ŭ������ ThreadHandlerBase�� ����
	// DBThread���� ������ DBCheck �ݹ��Լ� ���
	DBWork(&ThreadHandlerJoinMessage::DBCheck);
}
