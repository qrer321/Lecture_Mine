#include "PreCompile.h"
#include "DBConnecter.h"
#include <GameServerBase/GameServerDebug.h>


////////////////////////////////////////////////// DBStatement //////////////////////////////////////////////////
DBStatement::DBStatement(DBConnecter* connecter, MYSQL_STMT* statement, const std::string_view& query)
	: m_DBConnecter(connecter)
	, m_Statement(statement)
	, m_Query(query)
{
	m_ParamLengthBuffer.reserve(20);
	m_ParamIsNullBuffer.reserve(20);
	m_ParamBindBuffer.reserve(1024);

	m_ResultLengthBuffer.reserve(20);
	m_ResultIsNullBuffer.reserve(20);
	m_ResultBindBuffer.reserve(1024);
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

void DBStatement::Execute()
{
	const unsigned long param_count = mysql_stmt_param_count(m_Statement);

	// ���� SQL���� ���Ǵ� ? placeholder�� ������
	// �ڵ� �󿡼� Bind �Ϸ��� ������ ��ġ�ϴ��� Ȯ��
	if (param_count != m_ParamBinds.size())
	{
		GameServerDebug::AssertDebugMsg("Query Bind Count Is Not Equal");
		return;
	}

	if (0 != param_count)
	{
		if (0 != mysql_stmt_bind_param(m_Statement, &m_ParamBinds[0]))
		{
			GameServerDebug::AssertDebugMsg("mysql_stmt_bind_param Error" + m_DBConnecter->GetLastSQLError());
			return;
		}
	}

	MYSQL_RES* result_metadata = mysql_stmt_result_metadata(m_Statement);

	// ���������� SELECT�� �ƴ϶� UPDATE, DELETE���� �����ϱ⿡
	// ������� �������� ���� ���� �ִ�.
	if (nullptr != result_metadata)
	{
		// result_metadata�� nullptr�� �ƴ϶��
		// �ش� result_metadata�� column_count�� row_count�� �˾Ƴ��� �Ѵ�.
		const unsigned int column_count = mysql_num_fields(result_metadata);
		for (unsigned int i = 0; i < column_count; ++i)
		{
			MYSQL_BIND& result_bind = m_ResultBinds.emplace_back();
			m_ResultIsNullBuffer.emplace_back();
			m_ResultLengthBuffer.emplace_back();

			const MYSQL_FIELD& field = result_metadata->fields[i];
			const size_t buffer_start = m_ResultBinds.size();

			switch (field.type)
			{
			case MYSQL_TYPE_VAR_STRING:
				m_ResultBinds.resize(m_ResultBinds.size() + field.length + 1);

				result_bind.buffer_type = MYSQL_TYPE_VAR_STRING;

				result_bind.buffer = &m_ResultBinds[buffer_start];
				memset(result_bind.buffer, 0x00, field.length + 1);

				result_bind.is_null = reinterpret_cast<bool*>(&m_ResultIsNullBuffer[m_ResultIsNullBuffer.size() - 1]);
				*result_bind.is_null = false;

				result_bind.length = &m_ResultLengthBuffer[m_ResultLengthBuffer.size() - 1];
				break;

			case MYSQL_TYPE_LONG:
				m_ResultBinds.resize(m_ResultBinds.size() + field.length);

				result_bind.buffer_type = MYSQL_TYPE_LONG;

				result_bind.buffer = &m_ResultBinds[buffer_start];
				memset(result_bind.buffer, 0x00, field.length + 1);

				result_bind.is_null = reinterpret_cast<bool*>(&m_ResultIsNullBuffer[m_ResultIsNullBuffer.size() - 1]);
				*result_bind.is_null = false;

				result_bind.length = &m_ResultLengthBuffer[m_ResultLengthBuffer.size() - 1];
				break;

			default:
				GameServerDebug::AssertDebugMsg("Mysql Result Type Switch Error");
				break;
			}
		}

		if (0 != mysql_stmt_bind_result(m_Statement, &m_ResultBinds[0]))
		{
			GameServerDebug::AssertDebugMsg("mysql_stmt_bind_result Error" + m_DBConnecter->GetLastSQLError());
			return;
		}
	}

	// Query�� DB�� ������.
	if (0 != mysql_stmt_execute(m_Statement))
	{
		GameServerDebug::AssertDebugMsg("mysql_stmt_execute Error" + m_DBConnecter->GetLastSQLError());
		return;
	}

	// DB�� ���� ����� �޴´�.
	if (0 != mysql_stmt_store_result(m_Statement))
	{
		GameServerDebug::AssertDebugMsg("mysql_stmt_store_result Error" + m_DBConnecter->GetLastSQLError());
		return;
	}

	// ����� INSERT �Ǵ� UPDATE ���� ���� AUTO_INCREMENT ���� ������ ���� ��ȯ�Ѵ�.
	// AUTO_INCREMENT �ʵ尡 ���Ե� ���̺��� ����� INSERT ���� ������ �Ŀ� ����ϴ� �Լ�
	mysql_stmt_insert_id(m_Statement);

	// mysql_stmt_execute �Լ��� ���� ��ɹ� ���� ���Ŀ� ȣ�� �� �� �ִ�.
	// UPDATE, DELETE, INSERT ���� ��� ����, ���� �Ǵ� ���ε� �� ���� ��ȯ�Ѵ�.
	mysql_stmt_affected_rows(m_Statement);

	if (nullptr != result_metadata)
	{
		mysql_free_result(result_metadata);
	}
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