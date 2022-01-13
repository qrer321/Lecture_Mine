#include "PreCompile.h"
#include "GameServerQueue.h"
#include "GameServerDebug.h"
#include "GameServerThread.h"

void GameServerQueue::QueueFunction(const std::shared_ptr<GameServerIocpWorker>& work, GameServerQueue* self,
                                    const std::string& threadName)
{
	if (nullptr == self)
		GameServerDebug::AssertDebugMsg("ť ������ ������ �����߽��ϴ�");

	GameServerThread::ThreadNameSetting(threadName + std::to_string(work->GetIndex()));

	self->Run(work);
}

GameServerQueue::GameServerQueue()
= default;

GameServerQueue::GameServerQueue(WORK_TYPE type, int threadCount, const std::string& threadName)
{
	Initialize(type, threadCount, threadName);
}

GameServerQueue::~GameServerQueue()
= default;

GameServerQueue::GameServerQueue(GameServerQueue&& other) noexcept
{

}

void GameServerQueue::Run(const std::shared_ptr<GameServerIocpWorker>& work)
{
	while (QUEUE_RETURN::DESTROY != Work(work));
}

GameServerQueue::QUEUE_RETURN GameServerQueue::Work(const std::shared_ptr<GameServerIocpWorker>& work)
{
	if (nullptr == m_WorkFunction)
		return QUEUE_RETURN::DESTROY;

	return m_WorkFunction(work);
}

GameServerQueue::QUEUE_RETURN GameServerQueue::WorkDefault(const std::shared_ptr<GameServerIocpWorker>& work)
{
	switch (IocpWaitReturnType returnType = work->Wait())
	{
	case IocpWaitReturnType::RETURN_ERROR:
		GameServerDebug::AssertDebugMsg("IOCP�� ����Ÿ���� RETURN_ERROR �Դϴ�");
		break;
	case IocpWaitReturnType::RETURN_TIMEOUT:
		break;
	case IocpWaitReturnType::RETURN_OK:
	{
		switch (auto type = static_cast<WORK_MESSAGE_TYPE>(work->GetNumberOfBytes()))
		{
		case WORK_MESSAGE_TYPE::MSG_DESTROY:
			return QUEUE_RETURN::DESTROY;
		case WORK_MESSAGE_TYPE::MSG_POST:
		{
			// IocpWorker�� ���� CompletionKey�� PostTask*�� ����ȯ�Ͽ�
			// �ش� �۾��� ����ִ� �Լ������͸� ȣ���Ѵ�.
			std::unique_ptr<PostTask> JobTask = std::unique_ptr<PostTask>(work->GetConvertCompletionKey<PostTask*>());
			JobTask->task();
		}
		return QUEUE_RETURN::OK;
		default:
			break;
		}
	}
	break;
	default:
		break;
	}

	return QUEUE_RETURN::OK;
}

GameServerQueue::QUEUE_RETURN GameServerQueue::WorkExtension(const std::shared_ptr<GameServerIocpWorker>& work)
{
	return QUEUE_RETURN::OK;
}

void GameServerQueue::SetWorkType(WORK_TYPE type)
{
	switch (type)
	{
	case WORK_TYPE::Default:
		m_WorkFunction = std::bind(&GameServerQueue::WorkDefault, this, std::placeholders::_1);
		break;
	case WORK_TYPE::Extension:
		m_WorkFunction = std::bind(&GameServerQueue::WorkExtension, this, std::placeholders::_1);
		break;
	default: 
		break;
	}
}

void GameServerQueue::Initialize(WORK_TYPE type, int threadCount, const std::string& threadName)
{
	SetWorkType(type);
	m_Iocp.Initialize(std::bind(QueueFunction, std::placeholders::_1, this, threadName), threadCount, INFINITE);
}

void GameServerQueue::EnQueue(const std::function<void()>& callback)
{
	if (nullptr == callback)
	{
		GameServerDebug::AssertDebugMsg("�ݹ��� nullptr �Դϴ�");
		return;
	}

	std::unique_ptr<PostTask> postTask = std::make_unique<PostTask>();
	postTask->task = callback;

	m_Iocp.Post(static_cast<DWORD>(WORK_MESSAGE_TYPE::MSG_POST), reinterpret_cast<ULONG_PTR>(postTask.get()));
	postTask.release();
}

bool GameServerQueue::NetworkBind(SOCKET socket, const std::function<void(BOOL, DWORD, LPOVERLAPPED)>& callback) const
{
	if (nullptr == callback)
	{
		GameServerDebug::AssertDebugMsg("�ݹ��� nullptr �Դϴ�");
		return false;
	}

	std::unique_ptr<OverlappedTask> overTask = std::make_unique<OverlappedTask>();
	overTask->task = callback;

	if (false == m_Iocp.Bind(reinterpret_cast<HANDLE>(socket), reinterpret_cast<ULONG_PTR>(overTask.get())))
	{
		GameServerDebug::GetLastErrorPrint();
		return false;
	}

	overTask.release();

	return true;
}

void GameServerQueue::Destroy()
{
	for (size_t i = 0; i < m_Iocp.GetThreadCount(); ++i)
	{
		// default
		m_Iocp.Post(static_cast<DWORD>(WORK_MESSAGE_TYPE::MSG_DESTROY), 0);
		Sleep(1);
	}
}