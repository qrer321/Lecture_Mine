#include "PreCompile.h"
#include "DBQueue.h"
#include <GameServerNet/DBConnecter.h>
#include "GameServerCore.h"

DBQueue* DBQueue::s_Inst = new DBQueue();

DBQueue::~DBQueue()
{
	m_TaskQueue.Destroy();
}

std::mutex connecter_lock;
void InitializeDBConnecter(DBConnecter* connecter)
{
	std::lock_guard lock(connecter_lock);

	connecter->Reset();

	if (nullptr == connecter)
	{
		GameServerDebug::AssertDebugMsg("DBConnecter Is Nullptr");
		return;
	}

	if (false == connecter->Connect(
		GameServerCore::GetDBHost(),
		GameServerCore::GetDBUser(),
		GameServerCore::GetDBPw(),
		GameServerCore::GetDBSchema(),
		GameServerCore::GetDBPort()))
	{
		GameServerDebug::AssertDebugMsg("DB Connection Error" + connecter->GetLastSQLError());
		return;
	}
}

void DBQueue::Init()
{
	// ������ Thread���� ����� ������ thread_local�̶�� local data�� ����Ѵ�.
	s_Inst->m_TaskQueue.InitializeLocalData<DBConnecter>(GameServerQueue::WORK_TYPE::Default, 20, "DBThread", InitializeDBConnecter);
}

void DBQueue::Queue(const std::function<void()>& callback)
{
	s_Inst->m_TaskQueue.EnQueue(callback);
}

void DBQueue::Destroy()
{
	if (nullptr != s_Inst)
	{
		delete s_Inst;
		s_Inst = nullptr;
	}
}
