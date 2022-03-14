#include "PreCompile.h"
#include "DBQueue.h"

GameServerQueue DBQueue::s_TaskQueue = GameServerQueue();

void DBQueue::Init()
{
	// ������ Thread���� ����� ������ thread_local�̶�� local data�� ����Ѵ�.
	s_TaskQueue.InitializeLocalData<TestClass>(GameServerQueue::WORK_TYPE::Default, 20, "DBThread");
}

void DBQueue::Queue(const std::function<void()>& callback)
{
	s_TaskQueue.EnQueue(callback);
}

void DBQueue::Destroy()
{
	s_TaskQueue.Destroy();
}
