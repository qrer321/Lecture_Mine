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
		GameServerDebug::AssertDebugMsg("ť ������ ������ �����߽��ϴ�");

	GameServerThread::ThreadNameSetting(threadName + std::to_string(work->GetIndex()));
	
	/*
	 * GameServerQueue�� Initialize �� ��, �����Լ� QueueFunction�� ȣ��ǰ�
	 * threadCount ��ŭ�� ��������� Run �Լ��� ȣ��� ���̴�.
	 * �׸��� �ش� ��������� ������ WorkType�� ���� Work �Լ��� ȣ��� ���̴�.
	 * ���������� threadCount��ŭ�� ��������� GameServerIocpWorker�� Wait �Լ��� ����
	 * GetQueuedCompletionStatus �Լ��� ���� Post�� ��ٸ��� �ȴ�.
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

// ���� ���� �����
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