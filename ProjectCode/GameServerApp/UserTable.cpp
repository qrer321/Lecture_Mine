#include "PreCompile.h"
#include "UserTable.h"
#include <GameServerNet/DBConnecter.h>

std::string s_TableName = "user";

std::shared_ptr<UserRow> UserTable::GetUserData(const std::string& id)
{
	return nullptr;
}

UserTable_SelectIDFromUserInfo::UserTable_SelectIDFromUserInfo(std::string id)
	: DBQuery("SELECT idx, ID, PW, FROM unreal_server.user WHERE ID = ?")
	, m_ID(std::move(id))
{
}

bool UserTable_SelectIDFromUserInfo::ExecuteQuery()
{
	// DB Injection ���� �������� ������ ���� ���ؼ�
	// Dynamic Query���� �ƴ� Fragment Query������ �����.
	std::unique_ptr<DBStatement> statement = m_DBConnecter->CreateStatement(m_QueryString);

	statement->ParamBind_String(m_ID);
	statement->Execute();

	return true;
}