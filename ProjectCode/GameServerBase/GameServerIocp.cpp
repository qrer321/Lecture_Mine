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

GameServerIocp::GameServerIocp()
= default;

/*
 * std::function<void(std::shared_ptr<GameServerIocpWorker>)> func��
 * const std::function<void(std::shared_ptr<GameServerIocpWorker>)>& func�� ����
 *
 * const ���۷����� ����
 * �Լ��� �Ű������� ������ �����ϸ� �� ��ü�� �����Ǵµ� ����
 * const ���۷����� �����ϸ� ���������� �ʰ�, ���� ������ ��������� �ʴ´ٴ� ������ �ִ�.
 * �ᱹ ������ �����ϱ⿡ ����Ѵ�
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

	// IOCP �ڵ� ����
	m_IocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, NULL, threadCount);

	for (size_t i = 0; i < static_cast<size_t>(threadCount); ++i)
	{
		AddThread(func, time);
	}
}

void GameServerIocp::AddThread(const std::function<void(std::shared_ptr<GameServerIocpWorker>)>& func, DWORD time)
{
	// vector�� ���� thread list�� thread safe �Ͽ��� �Ѵ�.
	// thread safe ���� ������ AddThread �Լ��� ���ÿ� ȣ��Ǿ� ������ �߻��� ���ɼ��� ����
	// mutex lock�� ���� �� ���� �ϳ��� thread�� ������ �����ϵ��� ����
	m_IocpLock.lock();

	std::shared_ptr<GameServerIocpWorker> newWork = std::make_shared<GameServerIocpWorker>(m_IocpHandle, time, static_cast<DWORD>(m_ThreadWorkerList.size()));
	m_ThreadWorkerList.push_back(newWork);
	m_ThreadList.push_back(std::make_shared<GameServerThread>(func, newWork));

	// lock �Լ� ȣ�� ���Ŀ��� �ݵ�� unlock�� ȣ�����־�� �Ѵ�.
	m_IocpLock.unlock();
}

void GameServerIocp::Post(DWORD byteSize, ULONG_PTR data)
{
	PostQueuedCompletionStatus(m_IocpHandle, byteSize, data, nullptr);
}

bool GameServerIocp::Bind(HANDLE handle, ULONG_PTR key)
{
	// iocp�� ���� �ش� handle�� ����
	if (m_IocpHandle != CreateIoCompletionPort(handle, m_IocpHandle, key, 0))
		return false;

	return true;
}
