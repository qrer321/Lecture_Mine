#pragma once
#include <Windows.h>
#include <mutex>
#include <functional> 
#include "GameServerObjectBase.h"
#include "GameServerThread.h"

// �뵵 : 
// �з� :
// ÷�� :

enum class IocpWaitReturnType
{
	RETURN_ERROR,
	RETURN_TIMEOUT,
	RETURN_POST,
};

class GameServerIocpWorker
{
public:
	friend class GameServerIocp;

public:
	GameServerIocpWorker(HANDLE iocpHandle, DWORD time, size_t index);

private:
	HANDLE			IocpHandle;
	DWORD			NumberOfBytesTransferred;
	ULONG_PTR		CompletionKey;
	LPOVERLAPPED	lpOverlapped;
	DWORD			Time;
	size_t			Index;
	BOOL			LastWaitValue;

public:
	template<typename ConvertType>
	ConvertType ConvertKey() { return reinterpret_cast<ConvertType>(CompletionKey); }

public:
	size_t GetIndex() const { return Index; }

public:
	IocpWaitReturnType Wait();
};

class GameServerIocp : public GameServerObjectBase
{
private: // Member Var
	HANDLE m_Iocp{};
	std::mutex m_IocpLock;
	std::vector<std::shared_ptr<GameServerThread>> m_ThreadList;
	std::vector<std::shared_ptr<GameServerIocpWorker>> m_ThreadWorkerList;

public: // Default
	GameServerIocp();
	GameServerIocp(std::function<void(std::shared_ptr<GameServerIocpWorker>)> func, int threadCount);
	GameServerIocp(std::function<void(std::shared_ptr<GameServerIocpWorker>)> func, int threadCount, DWORD time);

	~GameServerIocp();

	GameServerIocp(const GameServerIocp& _Other) = delete;
	GameServerIocp(GameServerIocp&& _Other) noexcept;

public:
	GameServerIocp& operator=(const GameServerIocp& _Other) = delete;
	GameServerIocp& operator=(GameServerIocp&& _Other) = delete;

public:
	void Initialize(const std::function<void(std::shared_ptr<GameServerIocpWorker>)>& func, int threadCount, DWORD time);

public: // Member Function
	void AddThread(const std::function<void(std::shared_ptr<GameServerIocpWorker>)>& func, DWORD time);
	void Post(DWORD byteSize, ULONG_PTR data);
};

