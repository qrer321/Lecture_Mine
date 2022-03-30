#include "PreCompile.h"
#include "ThreadHandlerCreateCharacterMessage.h"
#include "CharacterTable.h"

void ThreadHandlerCreateCharacterMessage::DBCheck()
{
	CharacterTable_SelectNickname select_query(m_Message->m_Nickname);
	

	if (false == select_query.ExecuteQuery())
	{
		m_ResultMessage.m_Code = EGameServerCode::OK;
	}
	else
	{
		m_ResultMessage.m_Code = EGameServerCode::CreateCharacterError;
	}

	NetWork(&ThreadHandlerCreateCharacterMessage::ResultSend);
}

void ThreadHandlerCreateCharacterMessage::ResultSend()
{
	GameServerSerializer serializer;
	m_ResultMessage.Serialize(serializer);
	m_TCPSession->Send(serializer.GetData());
}

void ThreadHandlerCreateCharacterMessage::Start()
{
	if (nullptr == m_TCPSession)
	{
		GameServerDebug::LogError("Join TCPSession Error");
		return;
	}

	m_ResultMessage.m_Code = EGameServerCode::CreateCharacterError;
	DBWork(&ThreadHandlerCreateCharacterMessage::DBCheck);
}