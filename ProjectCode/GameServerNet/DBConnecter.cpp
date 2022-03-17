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

	// 현재 SQL에서 사용되는 ? placeholder의 개수와
	// 코드 상에서 Bind 하려는 개수가 일치하는지 확인
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

	// 쿼리문에서 SELECT뿐 아니라 UPDATE, DELETE문도 존재하기에
	// 결과값이 존재하지 않을 수도 있다.
	if (nullptr != result_metadata)
	{
		// result_metadata가 nullptr가 아니라면
		// 해당 result_metadata의 column_count와 row_count를 알아내야 한다.
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

	// Query를 DB로 날린다.
	if (0 != mysql_stmt_execute(m_Statement))
	{
		GameServerDebug::AssertDebugMsg("mysql_stmt_execute Error" + m_DBConnecter->GetLastSQLError());
		return;
	}

	// DB로 날린 결과를 받는다.
	if (0 != mysql_stmt_store_result(m_Statement))
	{
		GameServerDebug::AssertDebugMsg("mysql_stmt_store_result Error" + m_DBConnecter->GetLastSQLError());
		return;
	}

	// 예약된 INSERT 또는 UPDATE 문에 의해 AUTO_INCREMENT 열에 생성된 값을 반환한다.
	// AUTO_INCREMENT 필드가 포함된 테이블에서 예약된 INSERT 문을 실행한 후에 사용하는 함수
	mysql_stmt_insert_id(m_Statement);

	// mysql_stmt_execute 함수에 의해 명령문 실행 직후에 호출 할 수 있다.
	// UPDATE, DELETE, INSERT 문인 경우 변경, 삭제 또는 삽인된 행 수를 반환한다.
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