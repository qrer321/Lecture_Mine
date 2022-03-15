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

void DBConnecter::Query(const std::string& query)
{
	if (false == IsConnected() && false == Connect())
	{

	}
}