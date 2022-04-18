#include "PreCompile.h"
#include "ThreadHandlerLoginMessage.h"

#include "CharacterTable.h"
#include "UserTable.h"
#include "ContentsSystemEnum.h"
#include "ContentsUserData.h"

/*
 * DB���� ����, �޽��� ����, ��� ��Ŷ�� ������ ��� �ϵ���
 * ThreadHandlerLoginMessage Ŭ���� ���ο��� �̷�������ϰ�
 * �ܺη� �����Ǿ�� �ȵȴ�.
 *
 * DBQueue�� NetQueue�� GameServerQueue�� ���� �����Ѵ�
 */
void ThreadHandlerLoginMessage::DBCheck()
{
	// GameServerThread���� thread_local�� ����
	// ������ �����尡 ���� local ������ ������⿡
	// �ڵ� ������ �ش� thread�� Name�� ���� �� �ְ� �Ǿ���.
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

	// �θ� Ŭ������ ThreadHandlerBase�� ����
	// NetThread���� ������ ResultSend �ݹ��Լ� ���
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

	// LoginResultMessage �� LoginError�� �ʱ�ȭ
	m_ResultMessage.m_Code = EGameServerCode::LoginError;

	// �θ� Ŭ������ ThreadHandlerBase�� ����
	// DBThread���� ������ DBCheck �ݹ��Լ� ���
	DBWork(&ThreadHandlerLoginMessage::DBCheck);
}
