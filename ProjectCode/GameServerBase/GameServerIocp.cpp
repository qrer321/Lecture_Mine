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
 * std::function<void(std::shared_ptr<GameServerIocpWorker>)> func��
 * const std::function<void(std::shared_ptr<GameServerIocpWorker>)>& func�� ����
 *
 * const ���۷����� ����
 * �Լ��� �Ű������� ������ �����ϸ� �� ��ü�� �����ǰ� ������ �� �ִ°Ϳ� ����
 * const ���۷����� �����ϸ� ���������� �ʰ�, ���� ������ ��������� �ʴ´ٴ� ������ �ִ�.
 * �ᱹ ���ɰ� �������� �����ϱ⿡ ����Ѵ�
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

	// IOCP �ڵ� ����
	m_IocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, NULL, thread_count);

	for (int i = 0; i < thread_count; ++i)
	{
		AddThread(func, time, i);
	}
}

void GameServerIocp::AddThread(const std::function<void(std::shared_ptr<GameServerIocpWorker>)>& func, DWORD time, unsigned int order)
{
	// vector�� ���� thread list�� thread safe �Ͽ��� �Ѵ�.
	// thread safe ���� ������ AddThread �Լ��� ���ÿ� ȣ��Ǿ� ������ �߻��� ���ɼ��� ����
	// mutex lock�� ���� �� ���� �ϳ��� thread�� ������ �����ϵ��� ����
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
	// iocp�� ���� �ش� handle�� ����
	if (m_IocpHandle != CreateIoCompletionPort(handle, m_IocpHandle, key, 0))
		return false;

	return true;
}
