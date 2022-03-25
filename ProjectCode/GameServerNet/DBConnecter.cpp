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
	const size_t param_buffer_size = m_ParamBindBuffer.size();
	m_ParamBindBuffer.resize(param_buffer_size + value.size());

	MYSQL_BIND& bind = m_ParamBinds.emplace_back();
	bind.buffer_type = MYSQL_TYPE_VARCHAR;

	bind.buffer = &m_ParamBindBuffer[param_buffer_size];
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

	// STMT���� ���Ǵ� ? placeholder�� ������
	// �ڵ� �󿡼� Bind �Ϸ��� ������ ��ġ�ϴ��� Ȯ��
	if (param_count != m_ParamBinds.size())
	{
		GameServerDebug::AssertDebugMsg("Query Bind Count Is Not Equal");
		return nullptr;
	}

	if (0 != param_count)
	{
		// STMT�� ? Placeholder�� Parameter�� ���ε� �Ѵ�.
		if (0 != mysql_stmt_bind_param(m_Statement, &m_ParamBinds[0]))
		{
			GameServerDebug::AssertDebugMsg("mysql_stmt_bind_param Error" + m_DBConnecter->GetLastSQLError());
			return nullptr;
		}
	}

	MYSQL_RES* result_metadata = mysql_stmt_result_metadata(m_Statement);

	std::unique_ptr<DBStatementResult> stmt_result = nullptr;

	// ���������� SELECT�� �ƴ϶� UPDATE, DELETE���� �����ϱ⿡
	// ������� �������� ���� ���� �ִ�.
	if (nullptr != result_metadata)
	{
		stmt_result = std::make_unique<DBStatementResult>(m_DBConnecter, m_Statement, m_Query);

		// result_metadata�� nullptr�� �ƴ϶��
		// �ش� result_metadata�� column_count�� row_count�� �˾Ƴ��� �Ѵ�.
		const unsigned int column_count = mysql_num_fields(result_metadata);
		for (unsigned int i = 0; i < column_count; ++i)
		{
			MYSQL_BIND& result_bind = stmt_result->m_ResultBinds.emplace_back();
			stmt_result->m_ResultIsNullBuffer.emplace_back();
			stmt_result->m_ResultLengthBuffer.emplace_back();

			const size_t result_buffer_size = stmt_result->m_ResultBindBuffer.size();

			const MYSQL_FIELD& field = result_metadata->fields[i];
			switch (field.type)
			{
			case MYSQL_TYPE_VAR_STRING:
				stmt_result->m_ResultBindBuffer.resize(stmt_result->m_ResultBindBuffer.size() + field.length + 1);

				result_bind.buffer_type = MYSQL_TYPE_VAR_STRING;

				result_bind.buffer = &stmt_result->m_ResultBindBuffer[result_buffer_size];
				memset(result_bind.buffer, 0x00, field.length + 1);

				result_bind.buffer_length = field.length + 1;

				result_bind.is_null = reinterpret_cast<bool*>(&stmt_result->m_ResultIsNullBuffer[stmt_result->m_ResultIsNullBuffer.size() - 1]);
				*result_bind.is_null = false;

				result_bind.length = &stmt_result->m_ResultLengthBuffer[stmt_result->m_ResultLengthBuffer.size() - 1];
				break;

			case MYSQL_TYPE_LONG:
				stmt_result->m_ResultBindBuffer.resize(stmt_result->m_ResultBindBuffer.size() + field.length);

				result_bind.buffer_type = MYSQL_TYPE_LONG;

				result_bind.buffer = &stmt_result->m_ResultBindBuffer[result_buffer_size];
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

	// Query�� DB�� ������.
	if (0 != mysql_stmt_execute(m_Statement))
	{
		GameServerDebug::AssertDebugMsg("mysql_stmt_execute Error" + m_DBConnecter->GetLastSQLError());
		return nullptr;
	}

	// DB�� ���� ����� �޴´�.
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

void DBConnecter::Reset()
{
	m_mysql = nullptr;
	m_Host = std::string();
	m_ID = std::string();
	m_PW = std::string();
	m_Schema = std::string();
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

	// nullptr �Է½� mysql �ʱ�ȭ
	m_mysql = mysql_init(nullptr);

	if (nullptr == m_mysql)
	{
		GameServerDebug::AssertDebugMsg("Mysql Init Error");
		return false;
	}

	mysql_options(m_mysql, MYSQL_SET_CHARSET_NAME, "utf8");

	// ���ÿ� �������� SELECT ������ �޾ƺ� �� �ֵ��� ó��
	MYSQL* handle = mysql_real_connect(m_mysql, m_Host.c_str(), m_ID.c_str(),
		m_PW.c_str(), m_Schema.c_str(), m_Port, nullptr, CLIENT_MULTI_RESULTS);

	if (m_mysql != handle)
	{
		// ������� m_mysql�� �޾ƿ� handle�� �ٸ� ��� ����
		GameServerDebug::AssertDebugMsg(std::string("MySQL Connect Error -> ") + GetLastSQLError());
		return false;
	}

	// ���� DB���� ������ �������� ���
	// �ڵ� ���ο��� ������ �˾Ƽ� �����ϴ� �ɼ�
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
	// Stmt�� query�� mysql ���̺귯�� ���ؿ���
	// ����ϰ� ���� �� ����ϴ� ���� ����

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