#include "PreCompile.h"
#include "ThreadHandlerSelectCharacterMessage.h"

#include <GameServerCore/GameServerSectionManager.h>
#include "CharacterTable.h"
#include "ContentsSystemEnum.h"
#include "ContentsUserData.h"
#include "Player.h"

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
		for (const FCharacterInfo& character_info : user_data->m_CharactersInfo)
		{
			if (character_info.m_Nickname == m_Message->m_Nickname)
			{
				m_SelectCharacterInfo = character_info;

				GameServerSerializer serializer;
				m_ResultMessage.m_Nickname = m_Message->m_Nickname;
				m_ResultMessage.Serialize(serializer);
				m_TCPSession->Send(serializer.GetData());
				GameServerDebug::LogInfo("Send Select Character OK Result");

				NetWork(&ThreadHandlerSelectCharacterMessage::InsertSection);

				return;
			}
		}
	}

	GameServerSerializer serializer;
	m_ResultMessage.m_Code = EGameServerCode::SelectCharacterError;
	m_ResultMessage.Serialize(serializer);
	m_TCPSession->Send(serializer.GetData());

	GameServerDebug::LogInfo("Send Select Character Fail Result");
	NetWork(&ThreadHandlerSelectCharacterMessage::InsertSection);
}

void ThreadHandlerSelectCharacterMessage::InsertSection()
{
	const std::shared_ptr<GameServerSection> last_section = GameServerSectionManager::GetInst()->FindSection(m_SelectCharacterInfo.m_LastSectionID);
	if (nullptr == last_section)
	{
		return;
	}

	last_section->CreateActor<Player>(m_TCPSession);
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
