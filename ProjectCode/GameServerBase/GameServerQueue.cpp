#include "PreCompile.h"
#include "GameServerQueue.h"
#include "GameServerDebug.h"
#include "GameServerThread.h"

GameServerQueue::GameServerQueue()
= default;

GameServerQueue::GameServerQueue(WORK_TYPE type, int threadCount, const std::string& threadName)
{
	Initialize(type, threadCount, threadName);
}

GameServerQueue::~GameServerQueue()
{
	Destroy();
}

GameServerQueue::GameServerQueue(GameServerQueue&& other) noexcept
{

}

void GameServerQueue::QueueFunction(const std::shared_ptr<GameServerIocpWorker>& work, GameServerQueue* self,
	const std::string& threadName)
{
	if (nullptr == self)
		GameServerDebug::AssertDebugMsg("큐 스레드 생성에 실패했습니다");

	GameServerThread::ThreadNameSetting(threadName + std::to_string(work->GetIndex()));
	
	/*
	 * GameServerQueue를 Initialize 할 때, 정적함수 QueueFunction이 호출되고
	 * threadCount 만큼의 스레드들은 Run 함수가 호출될 것이다.
	 * 그리고 해당 스레드들은 설정된 WorkType에 맞춰 Work 함수가 호출될 것이다.
	 * 마지막으로 threadCount만큼의 스레드들은 GameServerIocpWorker의 Wait 함수를 통해
	 * GetQueuedCompletionStatus 함수로 인해 Post를 기다리게 된다.
	 */
	self->Run(work);
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
	// 클라이언트가 메세지를 보내면
	// PostQueuedCompletionStatue 함수가 호출될 것이고,
	// 대기하고 있던 스레드들은 해당 함수로 인하여 깨어나게 된다.
	BOOL returnType = work->Wait();

	IocpWaitReturnType checkType = IocpWaitReturnType::RETURN_OK;
	if (0 == returnType)
	{
		if (WAIT_TIMEOUT == GetLastError())
		{
			checkType = IocpWaitReturnType::RETURN_TIMEOUT;
		}
		checkType = IocpWaitReturnType::RETURN_ERROR;
	}

	switch (checkType)
	{
	case IocpWaitReturnType::RETURN_ERROR:
		GameServerDebug::AssertDebugMsg("IOCP의 리턴타입이 RETURN_ERROR 입니다");
		break;
	case IocpWaitReturnType::RETURN_TIMEOUT:
		break;
	case IocpWaitReturnType::RETURN_OK:
	{
		// ReturnType이 RETURN_OK일 경우,
		// Post한 메세지의 NumberOfBytes가 무엇이냐에 따라 다른 동작을 수행한다.
		// MSG_POST일 경우 동기 액셉트를 통해서 들어온 것이고,
		// Default일 경우 비동기 액셉트를 통해 들어온 것이다.
		switch (auto type = static_cast<WORK_MESSAGE_TYPE>(work->GetNumberOfBytes()))
		{
		case WORK_MESSAGE_TYPE::MSG_DESTROY:
			return QUEUE_RETURN::DESTROY;
		case WORK_MESSAGE_TYPE::MSG_POST:
		{
			if (0 != work->GetCompletionKey())
			{
				// IocpWorker에 들어온 CompletionKey를 PostTask*로 형변환하여
				// 해당 작업에 들어있는 함수포인터를 호출한다.
				std::unique_ptr<PostTask> postTask = std::unique_ptr<PostTask>(work->GetConvertCompletionKey<PostTask*>());
				postTask->task();
			}
			else
			{
				GameServerDebug::LogError("PostTask Is Null");
			}
		}
		return QUEUE_RETURN::OK;
		default:
			{
				if (0 != work->GetCompletionKey())
				{
					auto overTask = work->GetConvertCompletionKey<OverlappedTask*>();
					overTask->task(returnType, work->GetNumberOfBytes(), work->GetOverlappedPtr());
				}
				else
				{
					GameServerDebug::LogError("OverlappedTask Is Null");
				}
			}
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

// 동기 파일 입출력
void GameServerQueue::EnQueue(const std::function<void()>& callback)
{
	if (nullptr == callback)
	{
		GameServerDebug::AssertDebugMsg("콜백이 nullptr 입니다");
		return;
	}

	std::unique_ptr<PostTask> postTask = std::make_unique<PostTask>();
	postTask->task = callback;

	m_Iocp.Post(static_cast<DWORD>(WORK_MESSAGE_TYPE::MSG_POST), reinterpret_cast<ULONG_PTR>(postTask.get()));
	postTask.release();
}

// 비동기 파일 입출력
bool GameServerQueue::NetworkBind(SOCKET socket, const std::function<void(BOOL, DWORD, LPOVERLAPPED)>& callback) const
{
	if (nullptr == callback)
	{
		GameServerDebug::AssertDebugMsg("콜백이 nullptr 입니다");
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