#include "PreCompile.h"
#include "ThreadHandlerJoinMessage.h"
#include "UserTable.h"

void ThreadHandlerJoinMessage::DBCheck()
{
	UserTable_SelectIDFromUserInfo select_query(m_Message->m_ID);
	select_query.ExecuteQuery();

	if (nullptr != select_query.m_RowDatum)
	{
		// select_query를 통해 이미 DB내에 해당 ID가 존재하는 경우 Error
		m_ResultMessage.m_Code = EGameServerCode::JoinError;
	}
	else
	{
		UserTable_InsertToUserInfo insert_query(m_Message->m_ID, m_Message->m_PW);
		if (false == insert_query.ExecuteQuery())
		{
			// insert_query를 통해 DB에 INSERT 하였지만
			// 영향을 받은 Row가 없는 경우 Error
			m_ResultMessage.m_Code = EGameServerCode::JoinError;
		}
		else
		{
			// Insert 쿼리가 문제없이 동작했을 경우 Join OK
			m_ResultMessage.m_Code = EGameServerCode::OK;
		}
	}

	NetWork(&ThreadHandlerJoinMessage::ResultSend);
}

void ThreadHandlerJoinMessage::ResultSend()
{
	// 설정한 EGameServerCode를 Serialize하여 Session에 Send
	// 클라이언트에서는 해당 정보를 Deserialize하여 동작에 사용하여야 한다.
	GameServerSerializer serializer;
	m_ResultMessage.Serialize(serializer);
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
	m_ResultMessage.m_Code = EGameServerCode::JoinError;

	// 부모 클래스인 ThreadHandlerBase를 통해
	// DBThread에서 동작할 DBCheck 콜백함수 등록
	DBWork(&ThreadHandlerJoinMessage::DBCheck);
}
