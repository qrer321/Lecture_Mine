#pragma once
#include "GameServerNameBase.h"
#include "GameServerIocp.h"
#include "GameServerObjectPool.h"

// 첨언 : IOCP를 한번 감싸서 기본 규칙을 정의한 Queue
//		  관리되는 IOCP라고 본다.

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
		Default,	// 기본형, Post n번에 n번 리턴
		Extension	// 확장형, Post n번에 운영체제가 알아서 횟수를 정해 리턴
	};

private:
	// 일을 시키기
	struct PostTask : enable_shared_from_this<PostTask>
	{
		std::function<void()> task;
	};

	// 비동기 파일 입출력
	struct OverlappedTask : enable_shared_from_this<OverlappedTask>
	{
		// BOOL			: 성공/실패
		// DWORD		: 받은 패킷이나 처리한 데이터의 길이
		// LPOVERLAPPED : 비동기 입출력의 정보를 담고있는 OVERLAPPED 포인터
		std::function<void(BOOL returnType, DWORD NumberOfBytes, LPOVERLAPPED lpOverlapped)> task;
	};

	enum class QUEUE_RETURN : char
	{
		OK,
		DESTROY
	};

	enum class WORK_MESSAGE_TYPE : char
	{
		MSG_POST = -2,
		MSG_DESTROY = -1

		// 0 이상인 경우 서버 메시지
	};

	mutable GameServerObjectFindPool<OverlappedTask> m_IocpOverlappedTaskPool;

private:
	GameServerIocp m_Iocp;
	std::function<QUEUE_RETURN(std::shared_ptr<GameServerIocpWorker> work)> m_WorkFunction;

public: // Default
	GameServerQueue() = default;
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
	QUEUE_RETURN Work(const std::shared_ptr<GameServerIocpWorker>& work);
	QUEUE_RETURN WorkDefault(const std::shared_ptr<GameServerIocpWorker>& work);		// 한번에 하나씩
	QUEUE_RETURN WorkExtension(const std::shared_ptr<GameServerIocpWorker>& work);		// 한번에 여러개

	void SetWorkType(WORK_TYPE type);

public: // Member Function
	void Initialize(WORK_TYPE type, int thread_count, const std::string& thread_name);

	void EnQueue(const std::function<void()>& callback);
	bool NetworkBind(SOCKET socket, const std::function<void(BOOL, DWORD, LPOVERLAPPED)>& callback) const;

	void Destroy();

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

		/*if (nullptr != local_data)
		{
			delete local_data;
		}*/
	}
};

