#include "PreCompile.h"
#include "ThreadHandlerSelectCharacterMessage.h"
#include "CharacterTable.h"
#include "ContentsSystemEnum.h"
#include "ContentsUserData.h"

void ThreadHandlerSelectCharacterMessage::DBCheck()
{
	CharacterTable_SelectNickname select_query(m_Message->m_Nickname);
	if (false == select_query.ExecuteQuery())
	{
		m_ResultMessage.m_Code = EGameServerCode::SelectCharacterError;
		NetWork(&ThreadHandlerSelectCharacterMessage::SelectResult);
		return;
	}

	m_ResultMessage.m_Code = EGameServerCode::OK;
	NetWork(&ThreadHandlerSelectCharacterMessage::SelectResult);
}

void ThreadHandlerSelectCharacterMessage::SelectResult()
{
	std::shared_ptr<ContentsUserData> user_data = m_TCPSession->GetLink<ContentsUserData>(EDataIndex::USER_DATA);
	if (EGameServerCode::OK == m_ResultMessage.m_Code)
	{
		for (size_t i = 0; i < user_data->m_CharactersInfo.size(); ++i)
		{
			if (user_data->m_CharactersInfo[i].m_Nickname == m_Message->m_Nickname)
			{
				GameServerSerializer serializer;
				m_ResultMessage.m_Nickname = m_Message->m_Nickname;
				m_ResultMessage.Serialize(serializer);
				m_TCPSession->Send(serializer.GetData());
				GameServerDebug::LogInfo("Send Select Character OK Result");

				return;
			}
		}
	}

	GameServerSerializer serializer;
	m_ResultMessage.m_Code = EGameServerCode::SelectCharacterError;
	m_ResultMessage.Serialize(serializer);
	m_TCPSession->Send(serializer.GetData());

	GameServerDebug::LogInfo("Send Select Character Fail Result");
}

void ThreadHandlerSelectCharacterMessage::InsertSection()
{
}

void ThreadHandlerSelectCharacterMessage::Start()
{
	if (nullptr == m_TCPSession)
	{
		GameServerDebug::LogError("Select TCPSession Error");
		return;
	}

	//m_ResultMessage.m_Code = EGameServerCode::SelectCharacterError;
	DBWork(&ThreadHandlerSelectCharacterMessage::DBCheck);
}
