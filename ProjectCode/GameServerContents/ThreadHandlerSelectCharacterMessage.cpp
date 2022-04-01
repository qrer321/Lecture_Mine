#include "PreCompile.h"
#include "ThreadHandlerSelectCharacterMessage.h"
#include "CharacterTable.h"
#include "ContentsSystemEnum.h"
#include "ContentsUserData.h"

void ThreadHandlerSelectCharacterMessage::DBCheck()
{
	//const std::shared_ptr<ContentsUserData> user_data = m_TCPSession->GetLink<ContentsUserData>(EDataIndex::USER_DATA);
	//CharacterTable_SelectNickname select_query(m_Message->m_Nickname);
	
	//if (false == select_query.ExecuteQuery())
	//{
	//	m_SelectMessage.m_Code = EGameServerCode::OK;

	//	CharacterTable_CreateCharacter create_query = CharacterTable_CreateCharacter(m_Message->m_Nickname, user_data->m_UserData.m_Index);
	//}
	//else
	//{
	//	m_SelectMessage.m_Code = EGameServerCode::SelectCharacterError;
	//}

	NetWork(&ThreadHandlerSelectCharacterMessage::ResultSend);
}

void ThreadHandlerSelectCharacterMessage::ResultSend()
{
	//GameServerSerializer serializer;
	//m_SelectMessage.Serialize(serializer);
	//m_TCPSession->Send(serializer.GetData());

	GameServerDebug::LogInfo("Send Select Character Result");
}

void ThreadHandlerSelectCharacterMessage::Start()
{
	if (nullptr == m_TCPSession)
	{
		GameServerDebug::LogError("Select TCPSession Error");
		return;
	}

	//m_SelectMessage.m_Code = EGameServerCode::SelectCharacterError;
	DBWork(&ThreadHandlerSelectCharacterMessage::DBCheck);
}