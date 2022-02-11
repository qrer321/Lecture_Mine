#include "PreCompile.h"
#include "NetQueue.h"

GameServerQueue NetQueue::s_TaskQueue = GameServerQueue();

void NetQueue::Init()
{
	s_TaskQueue.Initialize(GameServerQueue::WORK_TYPE::Default, 20, "NetThread");
}

void NetQueue::Queue(const std::function<void()>& callback)
{
	s_TaskQueue.EnQueue(callback);
}

void NetQueue::Destroy()
{
	s_TaskQueue.Destroy();
}