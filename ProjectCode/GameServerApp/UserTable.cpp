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
	// DB Injection 등의 악의적인 공격을 막기 위해서
	// Dynamic Query문이 아닌 Fragment Query문으로 만든다.
	std::unique_ptr<DBStatement> statement = m_DBConnecter->CreateStatement(m_QueryString);

	statement->ParamBind_String(m_ID);
	statement->Execute();

	return true;
}