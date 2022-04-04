#include "PreCompile.h"
#include "ThreadHandlerCreateCharacterMessage.h"
#include "CharacterTable.h"
#include "ContentsStructure.h"
#include "ContentsSystemEnum.h"
#include "ContentsUserData.h"

void ThreadHandlerCreateCharacterMessage::DBCheck()
{
	const std::shared_ptr<ContentsUserData> user_data = m_TCPSession->GetLink<ContentsUserData>(EDataIndex::USER_DATA);
	CharacterTable_SelectNickname select_query(m_Message->m_Nickname);
	
	if (false == select_query.ExecuteQuery())
	{
		CharacterTable_CreateCharacter insert_query(m_Message->m_Nickname, user_data->m_UserData.m_Index);
		if (false == insert_query.ExecuteQuery())
		{
			m_ResultMessage.m_Code = EGameServerCode::CreateCharacterError;
		}
		else
		{
			CharacterTable_SelectNickname result_query(m_Message->m_Nickname);
			if (false == result_query.ExecuteQuery())
			{
				m_ResultMessage.m_Code = EGameServerCode::CreateCharacterError;
			}
			else
			{
				m_ResultMessage.m_Code = EGameServerCode::OK;
				m_ResultMessage.m_CharacterInfo.m_Index			= result_query.m_RowDatum->m_Index;
				m_ResultMessage.m_CharacterInfo.m_Nickname		= result_query.m_RowDatum->m_Nickname;
				m_ResultMessage.m_CharacterInfo.m_UserIndex		= result_query.m_RowDatum->m_UserIndex;
				m_ResultMessage.m_CharacterInfo.m_HP			= result_query.m_RowDatum->m_HP;
				m_ResultMessage.m_CharacterInfo.m_Att			= result_query.m_RowDatum->m_Att;
				m_ResultMessage.m_CharacterInfo.m_LastRoomID	= result_query.m_RowDatum->m_LastRoomID;
				m_ResultMessage.m_CharacterInfo.m_LastRoomPosX	= result_query.m_RowDatum->m_LastRoomPosX;
				m_ResultMessage.m_CharacterInfo.m_LastRoomPosY	= result_query.m_RowDatum->m_LastRoomPosY;
				m_ResultMessage.m_CharacterInfo.m_LastRoomPosZ	= result_query.m_RowDatum->m_LastRoomPosZ;
			}
		}
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