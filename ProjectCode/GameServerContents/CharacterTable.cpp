#include "PreCompile.h"
#include "CharacterTable.h"

CharacterTable_SelectNickname::CharacterTable_SelectNickname(std::string nickname)
	: DBQuery("SELECT NickName FROM userver2.characterinfo WHERE NickName = ? LIMIT 1")
	, m_Nickname(std::move(nickname))
{
}

bool CharacterTable_SelectNickname::ExecuteQuery()
{
	// DB Injection ���� �������� ������ ���� ���ؼ�
	// Dynamic Query���� �ƴ� Fragment Query������ �����.
	const std::unique_ptr<DBStatement> statement = m_DBConnecter->CreateStatement(m_QueryString);

	statement->ParamBind_String(m_Nickname);

	const std::unique_ptr stmt_result(statement->Execute());

	if (0 >= stmt_result->GetAffectedRow())
	{
		return false;
	}

	return true;
}
