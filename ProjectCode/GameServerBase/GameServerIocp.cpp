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
 * GetQueuedCompletionStatus() �Լ��� ���������� ����
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
	m_Iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, NULL, threadCount);

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

	std::shared_ptr<GameServerIocpWorker> newWork = std::make_shared<GameServerIocpWorker>(m_Iocp, time, static_cast<DWORD>(m_ThreadWorkerList.size()));
	m_ThreadWorkerList.push_back(newWork);
	m_ThreadList.push_back(std::make_shared<GameServerThread>(func, newWork));

	// lock �Լ� ȣ�� ���Ŀ��� �ݵ�� unlock�� ȣ�����־�� �Ѵ�.
	m_IocpLock.unlock();
}

void GameServerIocp::Post(DWORD byteSize, ULONG_PTR data)
{
	PostQueuedCompletionStatus(m_Iocp, byteSize, data, nullptr);
}