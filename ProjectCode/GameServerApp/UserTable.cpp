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
	const std::unique_ptr<DBStatement> statement = m_DBConnecter->CreateStatement(m_QueryString);

	statement->ParamBind_String(m_ID);

	const std::unique_ptr stmt_result(statement->Execute());

	if (1 != stmt_result->GetAffectedRow())
	{
		return false;
	}

	if (false == stmt_result->Next())
	{
		return false;
	}

	m_RowDatum = std::make_shared<UserRow>(stmt_result->GetInt(0), stmt_result->GetString(1), stmt_result->GetString(2));
	
	return true;
}

UserTable_AllUserInfo::UserTable_AllUserInfo()
	: DBQuery("SELECT * FROM unreal_server.user_info")
{
}

bool UserTable_AllUserInfo::ExecuteQuery()
{
	const std::unique_ptr<DBStatement> statement = m_DBConnecter->CreateStatement(m_QueryString);
	const std::unique_ptr stmt_result(statement->Execute());

	const uint64_t row_count = stmt_result->GetAffectedRow();
	m_RowData.reserve(row_count);

	while(stmt_result->Next())
	{
		m_RowData.push_back(std::make_shared<UserRow>(stmt_result->GetInt(0), stmt_result->GetString(1), stmt_result->GetString(2)));
	}

	return true;
}

UserTable_InsertToUserInfo::UserTable_InsertToUserInfo(std::string id, std::string pw)
	: DBQuery("INSERT INTO unreal_server.user_info (ID, PW) VALUES(?, ?)")
	, m_ID(std::move(id))
	, m_PW(std::move(pw))
{
}

bool UserTable_InsertToUserInfo::ExecuteQuery()
{
	const std::unique_ptr<DBStatement> statement = m_DBConnecter->CreateStatement(m_QueryString);

	statement->ParamBind_String(m_ID);
	statement->ParamBind_String(m_PW);

	statement->Execute();

	if (0 == statement->GetAffectedRow())
	{
		return false;
	}

	return true;
}
