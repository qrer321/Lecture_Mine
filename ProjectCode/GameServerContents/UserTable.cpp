#include "PreCompile.h"
#include "UserTable.h"
#include <GameServerNet/DBConnecter.h>

std::string s_TableName = "user";

UserTable_SelectIDFromUserInfo::UserTable_SelectIDFromUserInfo(std::string id)
	: DBQuery("SELECT * FROM userver2.userinfo WHERE ID = ? LIMIT 1")
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

UserTable_SelectAllUserInfo::UserTable_SelectAllUserInfo()
	: DBQuery("SELECT * FROM userver2.userinfo")
{
}

bool UserTable_SelectAllUserInfo::ExecuteQuery()
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
	: DBQuery("INSERT INTO userver2.userinfo (ID, PW) VALUES(?, ?)")
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

	if (static_cast<uint64_t>(-1) == statement->GetAffectedRow())
	{
		return false;
	}

	return true;
}
