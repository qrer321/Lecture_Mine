#include "PreCompile.h"
#include "DBQuery.h"
#include <GameServerBase/GameServerThread.h>

DBQuery::DBQuery(const char* query)
	: m_DBConnecter(GameServerThread::GetLocalData<DBConnecter>())
	, m_QueryString(query)
{
}
