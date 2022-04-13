#pragma once
#include "GameServerNameBase.h"
#include "GameServerIocp.h"
#include "GameServerObjectPool.h"

// ÷�� : IOCP�� �ѹ� ���μ� �⺻ ��Ģ�� ������ Queue
//		  �����Ǵ� IOCP��� ����.

enum class IocpWaitReturnType
{
	RETURN_ERROR,
	RETURN_TIMEOUT,
	RETURN_OK,
};

class GameServerQueue : public GameServerNameBase
{
public:
	enum class WORK_TYPE : char
	{
		Default,	// �⺻��, Post n���� n�� ����
		Extension	// Ȯ����, Post n���� �ü���� �˾Ƽ� Ƚ���� ���� ����
	};

	enum class QUEUE_RETURN : char
	{
		OK,
		TIME_OUT,
		EMPTY,
		DESTROY,
	};

private:
	// ���� ��Ű��
	struct PostTask : std::enable_shared_from_this<PostTask>
	{
		std::function<void()> task;
	};

	// �񵿱� ���� �����
	struct OverlappedTask : std::enable_shared_from_this<OverlappedTask>
	{
		// BOOL			: ����/����
		// DWORD		: ���� ��Ŷ�̳� ó���� �������� ����
		// LPOVERLAPPED : �񵿱� ������� ������ ����ִ� OVERLAPPED ������
		std::function<void(BOOL returnType, DWORD NumberOfBytes, LPOVERLAPPED lpOverlapped)> task;
	};

	enum class WORK_MESSAGE_TYPE : char
	{
		MSG_POST = -2,
		MSG_DESTROY = -1

		// 0 �̻��� ��� ���� �޽���
	};

	mutable GameServerObjectFindPool<OverlappedTask> m_IocpOverlappedTaskPool;

private:
	GameServerIocp m_Iocp;
	std::function<QUEUE_RETURN(std::shared_ptr<GameServerIocpWorker> work)> m_WorkFunction;
	std::function<QUEUE_RETURN(DWORD)>										m_ExecuteFunction;

public: // Default
	GameServerQueue();
	GameServerQueue(WORK_TYPE type, int threadCount, const std::string& threadName = "");
	~GameServerQueue() override;

	GameServerQueue(const GameServerQueue& other) = delete;
	GameServerQueue(GameServerQueue&& other) noexcept = delete;

public:
	GameServerQueue& operator=(const GameServerQueue& other) = delete;
	GameServerQueue& operator=(GameServerQueue&& other) = delete;

private:
	static void QueueFunction(const std::shared_ptr<GameServerIocpWorker>& work, GameServerQueue* self, const std::string& threadName);

	void Run(const std::shared_ptr<GameServerIocpWorker>& work);

	QUEUE_RETURN WorkThread(const std::shared_ptr<GameServerIocpWorker>& work);
	QUEUE_RETURN WorkDefault(const std::shared_ptr<GameServerIocpWorker>& work);		// �ѹ��� �ϳ���
	QUEUE_RETURN WorkExtension(const std::shared_ptr<GameServerIocpWorker>& work);		// �ѹ��� ������

	QUEUE_RETURN ExecuteDefault(DWORD time);
	QUEUE_RETURN ExecuteExtension(DWORD time);

	void SetWorkType(WORK_TYPE work_type);

public: // Member Function
	void Initialize(WORK_TYPE type, int thread_count, const std::string& thread_name);
	void Initialize(int thread_count);

	void EnQueue(const std::function<void()>& callback);
	bool NetworkBind(SOCKET socket, const std::function<void(BOOL, DWORD, LPOVERLAPPED)>& callback) const;

	void Destroy();

	void SetExecuteType(WORK_TYPE work_type);
	QUEUE_RETURN Execute(DWORD time);

	template <typename LocalDataType>
	void InitializeLocalData(const WORK_TYPE type, int thread_count, const std::string& thread_name, std::function<void(LocalDataType*)> init_function = nullptr)
	{
		SetWorkType(type);
		m_Iocp.Initialize([=](const auto& worker)
			{
				QueueFunctionLocalData<LocalDataType>(worker, this, thread_name, init_function);
			}, thread_count, INFINITE);

		//m_Iocp.Initialize(
		//	std::bind(QueueFunctionLocalData<LocalDataType>, std::placeholders::_1, this, thread_name, init_function),
		//	thread_count,
		//	INFINITE);
	}

	template <typename LocalDataType>
	static void QueueFunctionLocalData(const std::shared_ptr<GameServerIocpWorker>& work, GameServerQueue* self, const std::string& thread_name, std::function<void(LocalDataType*)> init_function)
	{
		if (nullptr == self)
		{
			GameServerDebug::AssertDebugMsg("Failed To Create Thread Socket");
			return;
		}

		GameServerThread::SetThreadName(thread_name + " " + std::to_string(work->GetIndex()));
		LocalDataType* local_data = GameServerThread::CreateThreadLocalData<LocalDataType>();

		if (nullptr != init_function)
		{
			init_function(local_data);
		}

		self->Run(work);
	}
};

