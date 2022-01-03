#include "PreCompile.h"
#include "GameServerIocp.h"
#include "GameServerDebug.h"

GameServerIocpWorker::GameServerIocpWorker(HANDLE iocpHandle, DWORD time, size_t index)
	: IocpHandle(iocpHandle)
	, NumberOfBytesTransferred(0)
	, CompletionKey(0)
	, lpOverlapped(nullptr)
	, Time(time)
	, Index(index)
	, LastWaitValue(FALSE)
{
}

/*
 * GetQueuedCompletionStatus() 함수를 내부적으로 구현
 *
 * @return
 * 0	: Time Out
 * 1	: Post
 * other : Error
 */
IocpWaitReturnType GameServerIocpWorker::Wait()
{
	LastWaitValue = GetQueuedCompletionStatus(IocpHandle, &NumberOfBytesTransferred, &CompletionKey, &lpOverlapped, Time);

	switch(LastWaitValue)
	{
	case 0:
		return IocpWaitReturnType::RETURN_TIMEOUT;
	case 1:
		return IocpWaitReturnType::RETURN_POST;
	default:
		break;
	}

	GameServerDebug::AssertDebug();
	return IocpWaitReturnType::RETURN_ERROR;
}

GameServerIocp::GameServerIocp()
{

}

GameServerIocp::GameServerIocp(std::function<void(std::shared_ptr<GameServerIocpWorker>)> func, int threadCount)
{
	Initialize(func, threadCount, INFINITE);
}

GameServerIocp::GameServerIocp(std::function<void(std::shared_ptr<GameServerIocpWorker>)> func, int threadCount,
	DWORD time)
{
	Initialize(func, threadCount, time);
}

GameServerIocp::~GameServerIocp() 
{

}

GameServerIocp::GameServerIocp(GameServerIocp&& _Other) noexcept
{
}

void GameServerIocp::Initialize(const std::function<void(std::shared_ptr<GameServerIocpWorker>)>& func, int threadCount,
                                DWORD time)
{
	if (128 <= threadCount)
		return;

	if (0 >= threadCount)
	{
		SYSTEM_INFO info;
		GetSystemInfo(&info);
		threadCount = static_cast<int>(info.dwNumberOfProcessors);
	}

	// IOCP 핸들 생성
	m_Iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, NULL, threadCount);

	for (size_t i = 0; i < static_cast<size_t>(threadCount); ++i)
	{
		AddThread(func, time);
	}
}

void GameServerIocp::AddThread(const std::function<void(std::shared_ptr<GameServerIocpWorker>)>& func, DWORD time)
{
	// vector에 들어가는 thread list는 thread safe 하여야 한다.
	// thread safe 하지 않으면 AddThread 함수가 동시에 호출되어 문제가 발생할 가능성이 존재
	// mutex lock을 통해 한 번에 하나의 thread만 접근이 가능하도록 설정
	m_IocpLock.lock();

	std::shared_ptr<GameServerIocpWorker> newWork = std::make_shared<GameServerIocpWorker>(m_Iocp, time, static_cast<DWORD>(m_ThreadWorkerList.size()));
	m_ThreadWorkerList.push_back(newWork);
	m_ThreadList.push_back(std::make_shared<GameServerThread>(func, newWork));

	// lock 함수 호출 이후에는 반드시 unlock을 호출해주어야 한다.
	m_IocpLock.unlock();
}

void GameServerIocp::Post(DWORD byteSize, ULONG_PTR data)
{
	PostQueuedCompletionStatus(m_Iocp, byteSize, data, nullptr);
}