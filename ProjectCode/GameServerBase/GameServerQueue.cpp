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
	 * GameServerQueue�� Initialize �� ��, �����Լ� QueueFunction�� ȣ��ǰ�
	 * threadCount ��ŭ�� ��������� Run �Լ��� ȣ��� ���̴�.
	 * �׸��� �ش� ��������� ������ WorkType�� ���� WorkThread �Լ��� ȣ��� ���̴�.
	 * ���������� threadCount��ŭ�� ��������� GameServerIocpWorker�� Wait �Լ��� ����
	 * GetQueuedCompletionStatus �Լ��� ���� Post�� ��ٸ��� �ȴ�.
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
	// Ŭ���̾�Ʈ�� �޼����� ������
	// PostQueuedCompletionStatue �Լ��� ȣ��� ���̰�,
	// ����ϰ� �ִ� ��������� �ش� �Լ��� ���Ͽ� ����� �ȴ�.
	BOOL returnType = work->Wait();

	// ReturnType�� RETURN_OK�� ���,
	// Post�� �޼����� NumberOfBytes�� �����̳Ŀ� ���� �ٸ� ������ �����Ѵ�.
	// MSG_POST�� ��� ���� �׼�Ʈ�� ���ؼ� ���� ���̰�,
	// Default�� ��� �񵿱� �׼�Ʈ�� ���� ���� ���̴�.
	switch (auto type = static_cast<WORK_MESSAGE_TYPE>(work->GetNumberOfBytes()))
	{
	case WORK_MESSAGE_TYPE::MSG_DESTROY:
		return QUEUE_RETURN::DESTROY;
	case WORK_MESSAGE_TYPE::MSG_POST:
	{
		if (0 != work->GetCompletionKey())
		{
			// IocpWorker�� ���� CompletionKey�� PostTask*�� ����ȯ�Ͽ�
			// �ش� �۾��� ����ִ� �Լ������͸� ȣ���Ѵ�.
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
	// auto&& work : ��������
	//				 LValue�� �� ���� �ְ� RValue�� �� ���� ������ �ǹ�
	// std::forward : perfect forwarding(�Ϻ��� ����), �ִ� �״�� �������شٶ�� �����ϸ� �ȴ�
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

// ���� ���� �����
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

// �񵿱� ���� �����
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