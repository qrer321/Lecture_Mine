#include "PreCompile.h"
#include "DBConnecterBase.h"
#include <GameServerBase/GameServerDebug.h>

void DBConnecterBase::Connect(const std::string& host, const std::string& id, const std::string& pw,
                              const std::string& schema, const unsigned int port)
{
	// nullptr �Է½� mysql �ʱ�ȭ
	m_mysql = mysql_init(nullptr);

	if (nullptr == m_mysql)
	{
		GameServerDebug::AssertDebugMsg("Mysql Init Error");
		return;
	}

	mysql_options(m_mysql, MYSQL_SET_CHARSET_NAME, "utf8");

	// ���ÿ� �������� SELECT ������ �޾ƺ� �� �ֵ��� ó��
	MYSQL* handle = mysql_real_connect(m_mysql, host.c_str(), id.c_str(),
		pw.c_str(), schema.c_str(), port, nullptr, CLIENT_MULTI_RESULTS);

	if (m_mysql != handle)
	{
		// ������� m_mysql�� �޾ƿ� handle�� �ٸ� ��� ����
		const char* error = mysql_error(m_mysql);
		GameServerDebug::AssertDebugMsg(std::string("MySQL Connect Error -> ") + error);
		return;
	}

	// ���� DB���� ������ �������� ���
	// �ڵ� ���ο��� ������ �˾Ƽ� �����ϴ� �ɼ�
	constexpr bool reconnect = true;
	mysql_options(m_mysql, MYSQL_OPT_RECONNECT, &reconnect);
}
