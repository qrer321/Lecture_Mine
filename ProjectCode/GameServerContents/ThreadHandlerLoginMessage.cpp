#include "PreCompile.h"
#include "ThreadHandlerLoginMessage.h"

#include "CharacterTable.h"
#include "UserTable.h"
#include "ContentsSystemEnum.h"
#include "ContentsUserData.h"

/*
 * DB로의 접속, 메시지 검증, 결과 패킷을 보내는 모든 일들은
 * ThreadHandlerLoginMessage 클래스 내부에서 이루어져야하고
 * 외부로 공개되어서는 안된다.
 *
 * DBQueue와 NetQueue는 GameServerQueue를 통해 동작한다
 */
void ThreadHandlerLoginMessage::DBCheck()
{
	// GameServerThread에서 thread_local을 통해
	// 각각의 스레드가 가질 local 변수를 만들었기에
	// 코드 상으로 해당 thread의 Name을 얻어올 수 있게 되었다.
	std::string thread_name = GameServerThread::GetThreadName();

	UserTable_SelectIDFromUserInfo select_query(m_Message->m_ID);
	select_query.ExecuteQuery();

	if (nullptr == select_query.m_RowDatum)
	{
		m_ResultMessage.m_Code = EGameServerCode::LoginError;
	}
	else
	{
		m_ResultMessage.m_Code = EGameServerCode::OK;
	}

	m_RowDatum = select_query.m_RowDatum;

	// 부모 클래스인 ThreadHandlerBase를 통해
	// NetThread에서 동작할 ResultSend 콜백함수 등록
	NetWork(&ThreadHandlerLoginMessage::ResultSend);
}

void ThreadHandlerLoginMessage::ResultSend()
{
	if (m_ResultMessage.m_Code == EGameServerCode::OK)
	{
		m_UserData = std::make_shared<ContentsUserData>();
		m_UserData->m_UserRow = *m_RowDatum;
		m_Session->SetLink(EDataIndex::USER_DATA, m_UserData);
		m_UserIndex = m_UserData->m_UserRow.m_Index;
	}

	GameServerSerializer serializer;
	m_ResultMessage.Serialize(serializer);
	m_Session->Send(serializer.GetData());

	GameServerDebug::LogInfo("Send Login Result");

	if (m_ResultMessage.m_Code == EGameServerCode::OK)
	{
		DBWork(&ThreadHandlerLoginMessage::DBCharacterListCheck);
	}
}

void ThreadHandlerLoginMessage::DBCharacterListCheck()
{
	m_UserData = m_Session->GetLink<ContentsUserData>(EDataIndex::USER_DATA);

	CharacterTable_SelectUserCharacters select_query(m_UserIndex);
	select_query.ExecuteQuery();

	m_CharacterListMessage.m_CharactersInfo.resize(select_query.m_RowData.size());
	for (size_t i = 0; i < select_query.m_RowData.size(); ++i)
	{
		m_CharacterListMessage.m_CharactersInfo[i] = select_query.m_RowData[i]->m_Info;
	}

	m_UserData->m_CharactersInfo = m_CharacterListMessage.m_CharactersInfo;

	NetWork(&ThreadHandlerLoginMessage::CharactersSend);
}

void ThreadHandlerLoginMessage::CharactersSend()
{
	GameServerSerializer serializer;
	m_CharacterListMessage.Serialize(serializer);
	m_Session->Send(serializer.GetData());

	if (EGameServerCode::OK == m_ResultMessage.m_Code)
	{
		const std::shared_ptr<ContentsUserData> user_data = m_Session->GetLink<ContentsUserData>(EDataIndex::USER_DATA);
		user_data->m_UserRow = *m_RowDatum;
	}

	GameServerDebug::LogInfo("Send Character List");
}

void ThreadHandlerLoginMessage::Start()
{
	if (nullptr == m_Session)
	{
		GameServerDebug::LogError("Login TCPSession Error");
		return;
	}

	// LoginResultMessage 값 LoginError로 초기화
	m_ResultMessage.m_Code = EGameServerCode::LoginError;

	// 부모 클래스인 ThreadHandlerBase를 통해
	// DBThread에서 동작할 DBCheck 콜백함수 등록
	DBWork(&ThreadHandlerLoginMessage::DBCheck);
}
