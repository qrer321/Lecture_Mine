#include "PreCompile.h"
#include "GameServerQueue.h"
#include "GameServerDebug.h"
#include "GameServerThread.h"

GameServerQueue::GameServerQueue()
	: m_WorkFunction(nullptr)
	, m_ExecuteFunction(nullptr)
{
}

GameServerQueue::GameServerQueue(WORK_TYPE type, int threadCount, const std::string& threadName)
{
	Initialize(type, threadCount, threadName);
}

GameServerQueue::~GameServerQueue()
{
	Destroy();
}

void GameServerQueue::QueueFunction(const std::shared_ptr<GameServerIocpWorker>& work, GameServerQueue* self,
	const std::string& threadName)
{
	if (nullptr == self)
	{
		GameServerDebug::AssertDebugMsg("Failed To Create Thread Socket");
		return;
	}

	GameServerThread::SetThreadName(threadName + std::to_string(work->GetIndex()));
	
	/*
	 * GameServerQueue를 Initialize 할 때, 정적함수 QueueFunction이 호출되고
	 * threadCount 만큼의 스레드들은 Run 함수가 호출될 것이다.
	 * 그리고 해당 스레드들은 설정된 WorkType에 맞춰 WorkThread 함수가 호출될 것이다.
	 * 마지막으로 threadCount만큼의 스레드들은 GameServerIocpWorker의 Wait 함수를 통해
	 * GetQueuedCompletionStatus 함수로 인해 Post를 기다리게 된다.
	 */
	self->Run(work);
}

void GameServerQueue::Initialize(const WORK_TYPE type, const int thread_count, const std::string& thread_name)
{
	SetWorkType(type);
	m_Iocp.Initialize([=](const auto& worker)
		{
			QueueFunction(std::move(worker), this, thread_name);
		}, thread_count, INFINITE);
}

void GameServerQueue::Initialize(int thread_count)
{
	m_Iocp.InitializeIocpHandle(thread_count);
}

void GameServerQueue::Run(const std::shared_ptr<GameServerIocpWorker>& work)
{
	while (QUEUE_RETURN::DESTROY != WorkThread(work));
}

GameServerQueue::QUEUE_RETURN GameServerQueue::WorkThread(const std::shared_ptr<GameServerIocpWorker>& work)
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

		return QUEUE_RETURN::OK;
	}
	default:  // NOLINT(clang-diagnostic-covered-switch-default)
	{
		if (0 != work->GetCompletionKey())
		{
			// @desc
			// task go to : TCPListener::OnAccept()
			//				TCPSession::OnCallBack()
			OverlappedTask* overTask = work->GetConvertCompletionKey<OverlappedTask*>();
			overTask->task(returnType, work->GetNumberOfBytes(), work->GetOverlappedPtr());
		}
		else
		{
			GameServerDebug::LogError("OverlappedTask Is Null");
		}
	}
	break;
	}

	return QUEUE_RETURN::OK;
}

GameServerQueue::QUEUE_RETURN GameServerQueue::WorkExtension(const std::shared_ptr<GameServerIocpWorker>& work)
{
	return QUEUE_RETURN::OK;
}

GameServerQueue::QUEUE_RETURN GameServerQueue::Execute(DWORD time)
{
	if (nullptr == m_ExecuteFunction)
	{
		GameServerDebug::AssertDebugMsg("Execute Mod Is Not Set");
	}

	return m_ExecuteFunction(time);
}

GameServerQueue::QUEUE_RETURN GameServerQueue::ExecuteDefault(DWORD time)
{
	DWORD number_of_bytes_transferred;
	ULONG_PTR completion_key;
	LPOVERLAPPED overlapped;

	const BOOL execute_result = m_Iocp.Execute(number_of_bytes_transferred, completion_key, overlapped, time);
	if (FALSE == execute_result)
	{
		if (WAIT_TIMEOUT == GetLastError())
		{
			return QUEUE_RETURN::TIME_OUT;
		}

		if (nullptr != overlapped)
		{
			if (0 != completion_key)
			{
				const OverlappedTask* overlapped_task = reinterpret_cast<OverlappedTask*>(completion_key);
				overlapped_task->task(execute_result, number_of_bytes_transferred, overlapped);
			}
		}

		return QUEUE_RETURN::EMPTY;
	}

	const int message_type = static_cast<int>(number_of_bytes_transferred);
	switch (message_type)
	{
	case -1:	// MSG_DESTROY
		return QUEUE_RETURN::DESTROY;

	case -2:	// MSG_POST
		if (0 != completion_key)
		{
			const PostTask* post_task = reinterpret_cast<PostTask*>(completion_key);
			post_task->task();
		}
		break;

	default:
		if (0 != completion_key)
		{
			const OverlappedTask* overlapped_task = reinterpret_cast<OverlappedTask*>(completion_key);
			overlapped_task->task(execute_result, number_of_bytes_transferred, overlapped);
		}
		break;
	}

	return QUEUE_RETURN::OK;
}

GameServerQueue::QUEUE_RETURN GameServerQueue::ExecuteExtension(DWORD time)
{
	OVERLAPPED_ENTRY overlapped_entry[32] = {};
	ULONG num_entries_removed = 0;
	QUEUE_RETURN return_result = QUEUE_RETURN::OK;

	const BOOL execute_result = m_Iocp.ExecuteEx(overlapped_entry, &num_entries_removed, time);
	if (FALSE == execute_result)
	{
		return QUEUE_RETURN::EMPTY;
	}

	for (size_t i = 0; i < num_entries_removed; ++i)
	{
		DWORD		 number_of_bytes_transferred	= overlapped_entry[i].dwNumberOfBytesTransferred;
		ULONG_PTR	 completion_key					= overlapped_entry[i].lpCompletionKey;
		LPOVERLAPPED overlapped						= overlapped_entry[i].lpOverlapped;

		const int message_type = static_cast<int>(number_of_bytes_transferred);
		switch (message_type)
		{
		case -1:	// MSG_DESTROY
			return_result = QUEUE_RETURN::DESTROY;
			break;
		case -2:	// MSG_POST
			if (0 != completion_key)
			{
				const PostTask* post_task = reinterpret_cast<PostTask*>(completion_key);
				post_task->task();
			}
			break;

		default:
			if (0 != completion_key)
			{
				const OverlappedTask* overlapped_task = reinterpret_cast<OverlappedTask*>(completion_key);
				overlapped_task->task(execute_result, number_of_bytes_transferred, overlapped);
			}
			break;
		}
	}

	return return_result;
}

void GameServerQueue::SetWorkType(const WORK_TYPE work_type)
{
	// auto&& work : 보편참조
	//				 LValue가 될 수도 있고 RValue가 될 수도 있음을 의미
	// std::forward : perfect forwarding(완벽한 전달), 있는 그대로 전달해준다라고 이해하면 된다
	switch (work_type)
	{
	case WORK_TYPE::Default:
		m_WorkFunction = [this](auto&& work) { return WorkDefault(std::forward<decltype(work)>(work)); };
		break;
	case WORK_TYPE::Extension:
		m_WorkFunction = [this](auto&& work) { return WorkExtension(std::forward<decltype(work)>(work)); };
		break;
	default: 
		break;
	}
}

void GameServerQueue::SetExecuteType(const WORK_TYPE work_type)
{
	switch (work_type)
	{
	case WORK_TYPE::Default:
		m_ExecuteFunction = [this](auto&& time) { return ExecuteDefault(std::forward<decltype(time)>(time)); };
		break;
	case WORK_TYPE::Extension:
		m_ExecuteFunction = [this](auto&& time) { return ExecuteExtension(std::forward<decltype(time)>(time)); };
		break;
	default:
		break;
	}
}

// 동기 파일 입출력
void GameServerQueue::EnQueue(const std::function<void()>& callback)
{
	if (nullptr == callback)
	{
		GameServerDebug::AssertDebugMsg("Callback Is Null");
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

	m_IocpOverlappedTaskPool.Push(overTask.get());
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