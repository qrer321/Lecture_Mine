#include "PreCompile.h"
#include "DBQueue.h"
#include <GameServerNet/DBConnecter.h>

GameServerQueue DBQueue::s_TaskQueue = GameServerQueue();

std::mutex connecter_lock;
void InitializeDBConnecter(DBConnecter* connecter)
{
	std::lock_guard lock(connecter_lock);

	if (nullptr == connecter)
	{
		GameServerDebug::AssertDebugMsg("DBConnecter Is Nullptr");
		return;
	}

	if (false == connecter->Connect("127.0.0.1", "root", "1234", "unreal_server", 3306))
	{
		GameServerDebug::AssertDebugMsg("DB Connection Error" + connecter->GetLastSQLError());
		return;
	}


}

void DBQueue::Init()
{
	// ������ Thread���� ����� ������ thread_local�̶�� local data�� ����Ѵ�.
	s_TaskQueue.InitializeLocalData<DBConnecter>(GameServerQueue::WORK_TYPE::Default, 20, "DBThread", InitializeDBConnecter);
}

void DBQueue::Queue(const std::function<void()>& callback)
{
	s_TaskQueue.EnQueue(callback);
}

void DBQueue::Destroy()
{
	s_TaskQueue.Destroy();
}
