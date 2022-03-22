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
		// select_query를 통해 이미 DB내에 해당 ID가 존재하는 경우 Error
		m_JoinResultMessage.m_Code = EGameServerCode::JoinError;
	}
	else
	{
		UserTable_InsertToUserInfo insert_query(m_JoinMessage->m_ID, m_JoinMessage->m_PW);
		if (false == insert_query.ExecuteQuery())
		{
			// insert_query를 통해 DB에 INSERT 하였지만
			// 영향을 받은 Row가 없는 경우 Error
			m_JoinResultMessage.m_Code = EGameServerCode::JoinError;
		}
		else
		{
			// Insert 쿼리가 문제없이 동작했을 경우 Join OK
			m_JoinResultMessage.m_Code = EGameServerCode::OK;
		}
	}

	// NetThread에서 동작할 ResultSend 콜백함수 등록
	NetQueue::Queue([self = shared_from_this()]
		{
			self->ResultSend();
		});
}

void ThreadHandlerJoinMessage::ResultSend()
{
	// 설정한 EGameServerCode를 Serialize하여 Session에 Send
	// 클라이언트에서는 해당 정보를 Deserialize하여 동작에 사용하여야 한다.
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

	// LoginResultMessage 값 JoinError로 초기화
	m_JoinResultMessage.m_Code = EGameServerCode::JoinError;

	// DBThread에서 동작할 DBCheck 콜백함수 등록
	DBQueue::Queue([self = shared_from_this()]
		{
			self->DBCheck();
		});
}
