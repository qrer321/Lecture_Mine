#include "PreCompile.h"
#include "CharacterTable.h"

CharacterTable_SelectNickname::CharacterTable_SelectNickname(std::string nickname)
	: DBQuery("SELECT Idx, NickName, UserIndex, ATT, HP, LastRoomID, LastRoomPosX, LastRoomPosY, LastRoomPosZ FROM userver2.characterinfo WHERE NickName = ? LIMIT 1")
	, m_Nickname(std::move(nickname))
{
}

bool CharacterTable_SelectNickname::ExecuteQuery()
{
	// DB Injection 등의 악의적인 공격을 막기 위해서
	// Dynamic Query문이 아닌 Fragment Query문으로 만든다.
	const std::unique_ptr<DBStatement> statement = m_DBConnecter->CreateStatement(m_QueryString);

	statement->ParamBind_String(m_Nickname);

	const std::unique_ptr stmt_result(statement->Execute());

	if (0 >= stmt_result->GetAffectedRow())
	{
		return false;
	}

	stmt_result->Next();

	m_RowDatum = std::make_shared<CharacterRow>(CharacterRow(
		stmt_result->GetInt(0),
		stmt_result->GetString(1),
		stmt_result->GetInt(2),
		stmt_result->GetFloat(3),
		stmt_result->GetFloat(4),
		stmt_result->GetInt(5),
		stmt_result->GetFloat(6),
		stmt_result->GetFloat(7),
		stmt_result->GetFloat(8)));

	return true;
}

CharacterTable_SelectUserCharacters::CharacterTable_SelectUserCharacters(int index)
	: DBQuery("SELECT Idx, NickName, UserIndex, ATT, HP, LastRoomID, LastRoomPosX, LastRoomPosY, LastRoomPosZ FROM userver2.characterinfo WHERE UserIndex = ?")
	, m_UserIndex(index)
{
}

bool CharacterTable_SelectUserCharacters::ExecuteQuery()
{
	const std::unique_ptr<DBStatement> statement = m_DBConnecter->CreateStatement(m_QueryString);
	statement->ParamBind_Int(m_UserIndex);

	const std::unique_ptr stmt_result(statement->Execute());
	const uint64_t row = stmt_result->GetAffectedRow();
	m_RowData.reserve(row);

	while (stmt_result->Next())
	{
		m_RowData.push_back(
			std::make_shared<CharacterRow>(CharacterRow(
				stmt_result->GetInt(0),
				stmt_result->GetString(1),
				stmt_result->GetInt(2),
				stmt_result->GetFloat(3),
				stmt_result->GetFloat(4),
				stmt_result->GetInt(5),
				stmt_result->GetFloat(6),
				stmt_result->GetFloat(7),
				stmt_result->GetFloat(8))));
	}

	return true;
}

CharacterTable_CreateCharacter::CharacterTable_CreateCharacter(std::string nickname, int id)
	: DBQuery("INSERT INTO userver2.characterinfo (NickName, UserIndex) VALUES (?, ?);")
	, m_Nickname(std::move(nickname))
	, m_Id(id)
{
}

bool CharacterTable_CreateCharacter::ExecuteQuery()
{
	const std::unique_ptr<DBStatement> statement = m_DBConnecter->CreateStatement(m_QueryString);
	statement->ParamBind_String(m_Nickname);
	statement->ParamBind_Int(m_Id);
	statement->Execute();

	// Select Query가 아닌 경우 stmt_result는 반환되지 않기에 statement 자체를 사용
	if (0 == statement->GetAffectedRow())
	{
		return false;
	}

	return true;
}
