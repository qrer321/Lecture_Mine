#include "PreCompile.h"
#include "DBQueue.h"

GameServerQueue DBQueue::s_TaskQueue = GameServerQueue();

void DBQueue::Init()
{
	// 각각의 Thread마다 사용할 정보는 thread_local이라는 local data를 사용한다.
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
