#include "PreCompile.h"
#include "ThreadHandlerLoginMessage.h"

#include <GameServerBase/GameServerString.h>
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
	std::string thread_name = GameServerThread::GetName();

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
		const std::shared_ptr<ContentsUserData> user_data = std::make_shared<ContentsUserData>();
		user_data->m_UserData = *m_RowDatum;
		m_TCPSession->SetLink(EDataIndex::USER_DATA, user_data);
		m_UserIndex = user_data->m_UserData.m_Index;
	}

	GameServerSerializer serializer;
	m_ResultMessage.Serialize(serializer);
	m_TCPSession->Send(serializer.GetData());

	GameServerDebug::LogInfo("Send Login Result");

	if (m_ResultMessage.m_Code == EGameServerCode::OK)
	{
		DBWork(&ThreadHandlerLoginMessage::DBCharacterListCheck);
	}
}

void ThreadHandlerLoginMessage::DBCharacterListCheck()
{
	CharacterTable_SelectUserCharacters select_query(m_UserIndex);
	select_query.ExecuteQuery();

	m_CharacterListMessage.m_Characters.resize(select_query.m_RowData.size());
	for (size_t i = 0; i < select_query.m_RowData.size(); ++i)
	{
		m_CharacterListMessage.m_Characters[i].m_Index = select_query.m_RowData[i]->m_Index;
		m_CharacterListMessage.m_Characters[i].m_Nickname = select_query.m_RowData[i]->m_Nickname;
		m_CharacterListMessage.m_Characters[i].m_UserIndex = select_query.m_RowData[i]->m_UserIndex;
		m_CharacterListMessage.m_Characters[i].m_HP = select_query.m_RowData[i]->m_HP;
		m_CharacterListMessage.m_Characters[i].m_Att = select_query.m_RowData[i]->m_Att;
		m_CharacterListMessage.m_Characters[i].m_LastRoomID = select_query.m_RowData[i]->m_LastRoomID;
		m_CharacterListMessage.m_Characters[i].m_LastRoomPosX = select_query.m_RowData[i]->m_LastRoomPosX;
		m_CharacterListMessage.m_Characters[i].m_LastRoomPosY = select_query.m_RowData[i]->m_LastRoomPosY;
		m_CharacterListMessage.m_Characters[i].m_LastRoomPosZ = select_query.m_RowData[i]->m_LastRoomPosZ;
	}

	NetWork(&ThreadHandlerLoginMessage::CharactersSend);
}

void ThreadHandlerLoginMessage::CharactersSend()
{
	GameServerSerializer serializer;
	m_CharacterListMessage.Serialize(serializer);
	m_TCPSession->Send(serializer.GetData());

	GameServerDebug::LogInfo("Send Character List");
}

void ThreadHandlerLoginMessage::Start()
{
	if (nullptr == m_TCPSession)
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
