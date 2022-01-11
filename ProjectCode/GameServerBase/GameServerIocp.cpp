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
	BOOL returnValue = GetQueuedCompletionStatus(IocpHandle, &NumberOfBytesTransferred, &CompletionKey, &lpOverlapped, Time);
	/*void(*Ptr)() = reinterpret_cast<void(*)()>(CompletionKey);
	Ptr();*/

	if (0 == returnValue)
	{
		if (WAIT_TIMEOUT == GetLastError())
			return IocpWaitReturnType::RETURN_TIMEOUT;

		return IocpWaitReturnType::RETURN_ERROR;
	}
	
	return IocpWaitReturnType::RETURN_OK;
}

GameServerIocp::GameServerIocp() = default;

/*
 * std::function<void(std::shared_ptr<GameServerIocpWorker>)> func와
 * const std::function<void(std::shared_ptr<GameServerIocpWorker>)>& func의 차이
 *
 * const 레퍼런스의 유무
 * 함수에 매개변수를 값으로 전달하면 값 전체가 복제되는데 비해
 * const 레퍼런스로 전달하면 복제되지도 않고, 원본 변수가 변경되지도 않는다는 장점이 있다.
 * 결국 성능이 증가하기에 사용한다
 */

GameServerIocp::GameServerIocp(const std::function<void(std::shared_ptr<GameServerIocpWorker>)>& func, int threadCount)
{
	Initialize(func, threadCount, INFINITE);
}

GameServerIocp::GameServerIocp(const std::function<void(std::shared_ptr<GameServerIocpWorker>)>& func, int threadCount,
                               DWORD time)
{
	Initialize(func, threadCount, time);
}

GameServerIocp::~GameServerIocp() 
{
	for (const auto& element : m_ThreadList)
	{
		element->join();
	}
}

GameServerIocp::GameServerIocp(GameServerIocp&& other) noexcept
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