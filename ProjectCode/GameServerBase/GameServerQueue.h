#pragma once

#include "GameServerNameBase.h"
#include "GameServerIocp.h"
#include "GameServerObjectPool.h"

// �뵵 : 
// �з� :
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

private:
	// ���� ��Ű��
	struct PostTask : enable_shared_from_this<PostTask>
	{
		std::function<void()> task;
	};

	// �񵿱� ���� �����
	struct OverlappedTask : enable_shared_from_this<OverlappedTask>
	{
		// BOOL			: ����/����
		// DWORD		: ���� ��Ŷ�̳� ó���� �������� ����
		// LPOVERLAPPED : �񵿱� ������� ������ ����ִ� OVERLAPPED ������
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

		// 0 �̻��� ��� ���� �޽���
	};

	mutable GameServerObjectFindPool<OverlappedTask> m_IocpOverlappedTaskPool;

private:
	GameServerIocp m_Iocp;
	std::function<QUEUE_RETURN(std::shared_ptr<GameServerIocpWorker> work)> m_WorkFunction;

public: // Default
	GameServerQueue();
	GameServerQueue(WORK_TYPE type, int threadCount, const std::string& threadName = "");
	~GameServerQueue() override;

	GameServerQueue(const GameServerQueue& other) = delete;
	GameServerQueue(GameServerQueue&& other) noexcept;

public:
	GameServerQueue& operator=(const GameServerQueue& other) = delete;
	GameServerQueue& operator=(GameServerQueue&& other) = delete;

private:
	static void QueueFunction(const std::shared_ptr<GameServerIocpWorker>& work, GameServerQueue* self, const std::string& threadName);

	void Run(const std::shared_ptr<GameServerIocpWorker>& work);
	QUEUE_RETURN Work(const std::shared_ptr<GameServerIocpWorker>& work);
	QUEUE_RETURN WorkDefault(const std::shared_ptr<GameServerIocpWorker>& work);		// �ѹ��� �ϳ���
	QUEUE_RETURN WorkExtension(const std::shared_ptr<GameServerIocpWorker>& work);		// �ѹ��� ������

	void SetWorkType(WORK_TYPE type);

public: // Member Function
	void Initialize(WORK_TYPE type, int threadCount, const std::string& threadName);

	void EnQueue(const std::function<void()>& callback);
	bool NetworkBind(SOCKET socket, const std::function<void(BOOL, DWORD, LPOVERLAPPED)>& callback) const;

	void Destroy();
};

