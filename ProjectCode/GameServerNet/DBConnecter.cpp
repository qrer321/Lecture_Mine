#include "PreCompile.h"
#include "DBConnecter.h"
#include <GameServerBase/GameServerDebug.h>

/////////////////////////////////////////////// DBStatementResult ///////////////////////////////////////////////
DBStatementResult::DBStatementResult(DBConnecter* connecter, MYSQL_STMT* statement, const std::string_view& query)
	: m_DBConnecter(connecter)
	, m_Statement(statement)
	, m_Query(query)
{
	m_ResultLengthBuffer.reserve(20);
	m_ResultIsNullBuffer.reserve(20);
	m_ResultBindBuffer.reserve(1024);
}

std::string DBStatementResult::GetString(const int index) const
{
	return { static_cast<char*>(m_ResultBinds[index].buffer) };
}

int DBStatementResult::GetInt(const int index) const
{
	return *static_cast<int*>(m_ResultBinds[index].buffer);
}

bool DBStatementResult::Next() const
{
	return 0 == mysql_stmt_fetch(m_Statement);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////// DBStatement //////////////////////////////////////////////////
DBStatement::DBStatement(DBConnecter* connecter, MYSQL_STMT* statement, const std::string_view& query)
	: m_DBConnecter(connecter)
	, m_Statement(statement)
	, m_Query(query)
{
	m_ParamLengthBuffer.reserve(20);
	m_ParamIsNullBuffer.reserve(20);
	m_ParamBindBuffer.reserve(1024);
}

void DBStatement::ParamBind_String(const std::string_view& value)
{
	m_ParamLengthBuffer.emplace_back();
	m_ParamIsNullBuffer.emplace_back();
	m_ParamBindBuffer.resize(m_ParamBindBuffer.size() + value.size());

	MYSQL_BIND& bind = m_ParamBinds.emplace_back();
	bind.buffer_type = MYSQL_TYPE_VARCHAR;

	bind.buffer = &m_ParamBindBuffer[m_ParamBindBuffer.size()];
	memset(bind.buffer, 0x00, value.size() + 1);
	memcpy_s(bind.buffer, value.length(), value.data(), value.length());

	bind.buffer_length = static_cast<unsigned long>(value.length());

	bind.is_null = reinterpret_cast<bool*>(&m_ParamIsNullBuffer[m_ParamIsNullBuffer.size() - 1]);
	*bind.is_null = false;

	bind.length = &m_ParamLengthBuffer[m_ParamLengthBuffer.size() - 1];
	*bind.length = bind.buffer_length;
}

void DBStatement::ParamBind_Int(const int value)
{
}

void DBStatement::ParamBind_Float(const float value)
{
}

std::unique_ptr<DBStatementResult> DBStatement::Execute()
{
	const unsigned long param_count = mysql_stmt_param_count(m_Statement);

	// STMT에서 사용되는 ? placeholder의 개수와
	// 코드 상에서 Bind 하려는 개수가 일치하는지 확인
	if (param_count != m_ParamBinds.size())
	{
		GameServerDebug::AssertDebugMsg("Query Bind Count Is Not Equal");
		return nullptr;
	}

	if (0 != param_count)
	{
		// STMT의 ? Placeholder에 Parameter를 바인드 한다.
		if (0 != mysql_stmt_bind_param(m_Statement, &m_ParamBinds[0]))
		{
			GameServerDebug::AssertDebugMsg("mysql_stmt_bind_param Error" + m_DBConnecter->GetLastSQLError());
			return nullptr;
		}
	}

	MYSQL_RES* result_metadata = mysql_stmt_result_metadata(m_Statement);

	std::unique_ptr<DBStatementResult> stmt_result = nullptr;

	// 쿼리문에서 SELECT뿐 아니라 UPDATE, DELETE문도 존재하기에
	// 결과값이 존재하지 않을 수도 있다.
	if (nullptr != result_metadata)
	{
		stmt_result = std::make_unique<DBStatementResult>(m_DBConnecter, m_Statement, m_Query);

		// result_metadata가 nullptr가 아니라면
		// 해당 result_metadata의 column_count와 row_count를 알아내야 한다.
		const unsigned int column_count = mysql_num_fields(result_metadata);
		for (unsigned int i = 0; i < column_count; ++i)
		{
			MYSQL_BIND& result_bind = stmt_result->m_ResultBinds.emplace_back();
			stmt_result->m_ResultIsNullBuffer.emplace_back();
			stmt_result->m_ResultLengthBuffer.emplace_back();

			const MYSQL_FIELD& field = result_metadata->fields[i];
			switch (field.type)
			{
			case MYSQL_TYPE_VAR_STRING:
				stmt_result->m_ResultBindBuffer.resize(stmt_result->m_ResultBindBuffer.size() + field.length + 1);

				result_bind.buffer_type = MYSQL_TYPE_VAR_STRING;

				result_bind.buffer = &stmt_result->m_ResultBindBuffer[stmt_result->m_ResultBindBuffer.size()];
				memset(result_bind.buffer, 0x00, field.length + 1);

				result_bind.buffer_length = field.length + 1;

				result_bind.is_null = reinterpret_cast<bool*>(&stmt_result->m_ResultIsNullBuffer[stmt_result->m_ResultIsNullBuffer.size() - 1]);
				*result_bind.is_null = false;

				result_bind.length = &stmt_result->m_ResultLengthBuffer[stmt_result->m_ResultLengthBuffer.size() - 1];
				break;

			case MYSQL_TYPE_LONG:
				stmt_result->m_ResultBindBuffer.resize(stmt_result->m_ResultBindBuffer.size() + field.length);

				result_bind.buffer_type = MYSQL_TYPE_LONG;

				result_bind.buffer = &stmt_result->m_ResultBindBuffer[stmt_result->m_ResultBindBuffer.size()];
				memset(result_bind.buffer, 0x00, sizeof(int));

				result_bind.buffer_length = sizeof(int);

				result_bind.is_null = reinterpret_cast<bool*>(&stmt_result->m_ResultIsNullBuffer[stmt_result->m_ResultIsNullBuffer.size() - 1]);
				*result_bind.is_null = false;

				result_bind.length = &stmt_result->m_ResultLengthBuffer[stmt_result->m_ResultLengthBuffer.size() - 1];
				break;

			default:
				GameServerDebug::AssertDebugMsg("Mysql Result Type Switch Error");
				break;
			}
		}

		if (0 != mysql_stmt_bind_result(m_Statement, &stmt_result->m_ResultBinds[0]))
		{
			GameServerDebug::AssertDebugMsg("mysql_stmt_bind_result Error" + m_DBConnecter->GetLastSQLError());
			return nullptr;
		}
	}

	// Query를 DB로 날린다.
	if (0 != mysql_stmt_execute(m_Statement))
	{
		GameServerDebug::AssertDebugMsg("mysql_stmt_execute Error" + m_DBConnecter->GetLastSQLError());
		return nullptr;
	}

	// DB로 날린 결과를 받는다.
	if (0 != mysql_stmt_store_result(m_Statement))
	{
		GameServerDebug::AssertDebugMsg("mysql_stmt_store_result Error" + m_DBConnecter->GetLastSQLError());
		return nullptr;
	}

	if (nullptr != result_metadata)
	{
		mysql_free_result(result_metadata);
	}

	return stmt_result;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////// DBConnecter //////////////////////////////////////////////////
std::string DBConnecter::GetLastSQLError() const
{
	return mysql_error(m_mysql);
}

void DBConnecter::SetConnectInfo(const std::string& host, const std::string& id, const std::string& pw,
	const std::string& schema, const unsigned int port)
{
	m_Host = host;
	m_ID = id;
	m_PW = pw;
	m_Schema = schema;
	m_Port = port;
}

bool DBConnecter::Connect()
{
	if (nullptr != m_mysql)
	{
		return true;
	}

	// nullptr 입력시 mysql 초기화
	m_mysql = mysql_init(nullptr);

	if (nullptr == m_mysql)
	{
		GameServerDebug::AssertDebugMsg("Mysql Init Error");
		return false;
	}

	mysql_options(m_mysql, MYSQL_SET_CHARSET_NAME, "utf8");

	// 동시에 여러개의 SELECT 정보를 받아볼 수 있도록 처리
	MYSQL* handle = mysql_real_connect(m_mysql, m_Host.c_str(), m_ID.c_str(),
		m_PW.c_str(), m_Schema.c_str(), m_Port, nullptr, CLIENT_MULTI_RESULTS);

	if (m_mysql != handle)
	{
		// 멤버변수 m_mysql과 받아온 handle이 다른 경우 에러
		GameServerDebug::AssertDebugMsg(std::string("MySQL Connect Error -> ") + GetLastSQLError());
		return false;
	}

	// 만약 DB와의 연결이 끊어지는 경우
	// 핸들 내부에서 연결을 알아서 복구하는 옵션
	constexpr bool reconnect = true;
	mysql_options(m_mysql, MYSQL_OPT_RECONNECT, &reconnect);

	return true;
}

bool DBConnecter::Connect(const std::string& host, const std::string& id, const std::string& pw,
	const std::string& schema, const unsigned int port)
{
	SetConnectInfo(host, id, pw, schema, port);
	return Connect();
}

std::unique_ptr<DBStatement> DBConnecter::CreateStatement(const std::string_view& query)
{
	// Stmt는 query를 mysql 라이브러리 수준에서
	// 방어하고 싶을 때 사용하는 쿼리 형식

	MYSQL_STMT* statement = mysql_stmt_init(m_mysql);
	if (nullptr == statement)
	{
		GameServerDebug::AssertDebugMsg("mysql_stmt_init Error");
		return nullptr;
	}

	if (0 != mysql_stmt_prepare(statement, query.data(), static_cast<unsigned long>(query.length())))
	{
		mysql_stmt_close(statement);
		GameServerDebug::AssertDebugMsg("mysql_stmt_prepare Error");
		return nullptr;
	}

	return std::make_unique<DBStatement>(this, statement, query);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////