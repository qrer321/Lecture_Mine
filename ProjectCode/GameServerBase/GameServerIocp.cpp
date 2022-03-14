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

BOOL GameServerIocpWorker::Wait()
{
	return GetQueuedCompletionStatus(IocpHandle, &NumberOfBytesTransferred, &CompletionKey, &lpOverlapped, Time);
}

/*
 * std::function<void(std::shared_ptr<GameServerIocpWorker>)> func와
 * const std::function<void(std::shared_ptr<GameServerIocpWorker>)>& func의 차이
 *
 * const 레퍼런스의 유무
 * 함수에 매개변수를 값으로 전달하면 값 전체가 복제되는데 비해
 * const 레퍼런스로 전달하면 복제되지도 않고, 원본 변수가 변경되지도 않는다는 장점이 있다.
 * 결국 성능이 증가하기에 사용한다
 */

GameServerIocp::GameServerIocp(const std::function<void(std::shared_ptr<GameServerIocpWorker>)>& func, int thread_count)
{
	Initialize(func, thread_count, INFINITE);
}

GameServerIocp::GameServerIocp(const std::function<void(std::shared_ptr<GameServerIocpWorker>)>& func, int thread_count, DWORD time)
{
	Initialize(func, thread_count, time);
}

GameServerIocp::~GameServerIocp()
{
	for (const auto& element : m_ThreadList)
	{
		element->join();
	}
}

void GameServerIocp::Initialize(const std::function<void(std::shared_ptr<GameServerIocpWorker>)>& func, int thread_count, DWORD time)
{
	if (128 <= thread_count)
		return;

	if (0 >= thread_count)
	{
		SYSTEM_INFO info;
		GetSystemInfo(&info);
		thread_count = static_cast<int>(info.dwNumberOfProcessors);
	}

	// IOCP 핸들 생성
	m_IocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, NULL, thread_count);

	for (int i = 0; i < thread_count; ++i)
	{
		AddThread(func, time, i);
	}
}

void GameServerIocp::AddThread(const std::function<void(std::shared_ptr<GameServerIocpWorker>)>& func, DWORD time, unsigned int order)
{
	// vector에 들어가는 thread list는 thread safe 하여야 한다.
	// thread safe 하지 않으면 AddThread 함수가 동시에 호출되어 문제가 발생할 가능성이 존재
	// mutex lock을 통해 한 번에 하나의 thread만 접근이 가능하도록 설정
	std::lock_guard lock(m_IocpLock);

	auto new_work = std::make_shared<GameServerIocpWorker>(m_IocpHandle, time, static_cast<DWORD>(m_ThreadWorkerList.size()));
	auto new_thread = std::make_shared<GameServerThread>(func, new_work);
	new_thread->SetThreadOrder(order);

	m_ThreadWorkerList.push_back(new_work);
	m_ThreadList.push_back(new_thread);
}

void GameServerIocp::Post(DWORD byteSize, ULONG_PTR data)
{
	PostQueuedCompletionStatus(m_IocpHandle, byteSize, data, nullptr);
}

bool GameServerIocp::Bind(HANDLE handle, ULONG_PTR key) const
{
	// iocp를 통해 해당 handle을 감시
	if (m_IocpHandle != CreateIoCompletionPort(handle, m_IocpHandle, key, 0))
		return false;

	return true;
}
