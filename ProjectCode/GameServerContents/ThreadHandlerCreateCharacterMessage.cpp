#include "PreCompile.h"
#include "ThreadHandlerCreateCharacterMessage.h"
#include "CharacterTable.h"
#include "ContentsSystemEnum.h"
#include "ContentsUserData.h"

void ThreadHandlerCreateCharacterMessage::DBCheck()
{
	const std::shared_ptr<ContentsUserData> user_data = m_TCPSession->GetLink<ContentsUserData>(EDataIndex::USER_DATA);
	CharacterTable_SelectNickname select_query(m_Message->m_Nickname);
	
	if (false == select_query.ExecuteQuery())
	{
		m_ResultMessage.m_Code = EGameServerCode::OK;

		CharacterTable_CreateCharacter create_query = CharacterTable_CreateCharacter(m_Message->m_Nickname, user_data->m_UserData.m_Index);
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

	GameServerDebug::LogInfo("Send Create Character Result");
}

void ThreadHandlerCreateCharacterMessage::Start()
{
	if (nullptr == m_TCPSession)
	{
		GameServerDebug::LogError("Create Character TCPSession Error");
		return;
	}

	m_ResultMessage.m_Code = EGameServerCode::CreateCharacterError;
	DBWork(&ThreadHandlerCreateCharacterMessage::DBCheck);
}