#include "PreCompile.h"
#include "DBConnecter.h"
#include <GameServerBase/GameServerDebug.h>

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

void DBConnecter::Query(const std::string& query)
{
	if (false == IsConnected() && false == Connect())
	{

	}
}