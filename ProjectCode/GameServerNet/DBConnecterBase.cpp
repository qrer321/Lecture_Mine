#include "PreCompile.h"
#include "DBConnecterBase.h"
#include <GameServerBase/GameServerDebug.h>

void DBConnecterBase::Connect(const std::string& host, const std::string& id, const std::string& pw,
                              const std::string& schema, const unsigned int port)
{
	// nullptr 입력시 mysql 초기화
	m_mysql = mysql_init(nullptr);

	if (nullptr == m_mysql)
	{
		GameServerDebug::AssertDebugMsg("Mysql Init Error");
		return;
	}

	mysql_options(m_mysql, MYSQL_SET_CHARSET_NAME, "utf8");

	// 동시에 여러개의 SELECT 정보를 받아볼 수 있도록 처리
	MYSQL* handle = mysql_real_connect(m_mysql, host.c_str(), id.c_str(),
		pw.c_str(), schema.c_str(), port, nullptr, CLIENT_MULTI_RESULTS);

	if (m_mysql != handle)
	{
		// 멤버변수 m_mysql과 받아온 handle이 다른 경우 에러
		const char* error = mysql_error(m_mysql);
		GameServerDebug::AssertDebugMsg(std::string("MySQL Connect Error -> ") + error);
		return;
	}

	// 만약 DB와의 연결이 끊어지는 경우
	// 핸들 내부에서 연결을 알아서 복구하는 옵션
	constexpr bool reconnect = true;
	mysql_options(m_mysql, MYSQL_OPT_RECONNECT, &reconnect);
}
