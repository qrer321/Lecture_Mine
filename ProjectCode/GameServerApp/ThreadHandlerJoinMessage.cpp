#include "PreCompile.h"
#include "ThreadHandlerJoinMessage.h"
#include <GameServerBase/GameServerDebug.h>
#include "DBQueue.h"
#include "NetQueue.h"
#include "UserTable.h"

ThreadHandlerJoinMessage::ThreadHandlerJoinMessage(std::shared_ptr<TCPSession> tcp_session, std::shared_ptr<JoinMessage> join_message)
	: m_TCPSession(std::move(tcp_session))
	, m_JoinMessage(std::move(join_message))
{
}

void ThreadHandlerJoinMessage::DBCheck()
{
	UserTable_SelectIDFromUserInfo select_query(m_JoinMessage->m_ID);
	select_query.ExecuteQuery();

	if (nullptr != select_query.m_RowDatum)
	{
		// select_query�� ���� �̹� DB���� �ش� ID�� �����ϴ� ��� Error
		m_JoinResultMessage.m_Code = EGameServerCode::JoinError;
	}
	else
	{
		UserTable_InsertToUserInfo insert_query(m_JoinMessage->m_ID, m_JoinMessage->m_PW);
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

	// NetThread���� ������ ResultSend �ݹ��Լ� ���
	NetQueue::Queue([self = shared_from_this()]
		{
			self->ResultSend();
		});
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

	// DBThread���� ������ DBCheck �ݹ��Լ� ���
	DBQueue::Queue([self = shared_from_this()]
		{
			self->DBCheck();
		});
}
