#include "PreCompile.h"
#include "ThreadHandlerSelectCharacterMessage.h"

#include <GameServerCore/GameServerSectionManager.h>
#include "CharacterTable.h"
#include "ContentsSystemEnum.h"
#include "ContentsUserData.h"
#include "ContentsPlayerData.h"
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
	const std::shared_ptr<ContentsUserData> user_data = m_TCPSession->GetLink<ContentsUserData>(EDataIndex::USER_DATA);
	if (EGameServerCode::OK == m_ResultMessage.m_Code)
	{
		for (const FCharacterInfo& character_info : user_data->m_CharactersInfo)
		{
			if (character_info.m_Nickname == m_Message->m_Nickname)
			{
				user_data->m_SelectCharacterInfo = character_info;

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
}

void ThreadHandlerSelectCharacterMessage::InsertSection()
{
	const std::shared_ptr<ContentsUserData> user_data = m_TCPSession->GetLink<ContentsUserData>(EDataIndex::USER_DATA);

	int last_section_id = user_data->m_SelectCharacterInfo.m_LastSectionID;
	if (-1 == last_section_id)
	{
		last_section_id = static_cast<int>(ESectionType::NONE_FIGHT);
	}

	const std::shared_ptr<GameServerSection> last_section = GameServerSectionManager::GetInst()->FindSection(last_section_id);
	if (nullptr == last_section)
	{
		return;
	}

	std::shared_ptr<Player> new_player = last_section->CreateSessionActor<Player>(m_TCPSession);
	if (nullptr == new_player)
	{
		// 액터 생성에 실패에 대한 정보를 클라이언트로 보내주어야 한다.
	}

	const std::shared_ptr<ContentsPlayerData> player_data = m_TCPSession->CreateLink<ContentsPlayerData>(EDataIndex::PLAYABLE);
	player_data->m_ConnectPlayer = new_player.get();	// 순환참조로 인해 그냥 포인터로 처리
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
