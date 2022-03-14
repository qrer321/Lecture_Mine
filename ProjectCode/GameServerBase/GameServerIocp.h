#pragma once
#include <WinSock2.h>
#include <Windows.h>
#include <mutex>
#include <functional> 
#include "GameServerObjectBase.h"
#include "GameServerThread.h"

// �뵵 : 
// �з� :
// ÷�� :

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
	ConvertType GetConvertCompletionKey() { return reinterpret_cast<ConvertType>(CompletionKey); }

public:
	[[nodiscard]] ULONG_PTR		GetCompletionKey() const { return CompletionKey; }
	[[nodiscard]] DWORD			GetNumberOfBytes() const { return NumberOfBytesTransferred; }
	[[nodiscard]] LPOVERLAPPED	GetOverlappedPtr() const { return lpOverlapped;  }

	[[nodiscard]] size_t		GetIndex()		   const { return Index; }

public:
	BOOL Wait();
};

class GameServerIocp : public GameServerObjectBase
{
private: // Member Var
	HANDLE m_IocpHandle{};
	std::mutex m_IocpLock;
	std::vector<std::shared_ptr<GameServerThread>> m_ThreadList;
	std::vector<std::shared_ptr<GameServerIocpWorker>> m_ThreadWorkerList;

public: // Default
	GameServerIocp() = default;
	GameServerIocp(const std::function<void(std::shared_ptr<GameServerIocpWorker>)>& func, int thread_count);
	GameServerIocp(const std::function<void(std::shared_ptr<GameServerIocpWorker>)>& func, int thread_count, DWORD time);
	~GameServerIocp() override;

	GameServerIocp(const GameServerIocp& other) = delete;
	GameServerIocp(GameServerIocp&& other) noexcept = delete;

public:
	GameServerIocp& operator=(const GameServerIocp& other) = delete;
	GameServerIocp& operator=(GameServerIocp&& other) = delete;

public:
	size_t GetThreadCount() const { return m_ThreadList.size(); }

	void Initialize(const std::function<void(std::shared_ptr<GameServerIocpWorker>)>& func, int thread_count, DWORD time);

	void AddThread(const std::function<void(std::shared_ptr<GameServerIocpWorker>)>& func, DWORD time, unsigned int order);
	void Post(DWORD byteSize, ULONG_PTR data);
	bool Bind(HANDLE handle, ULONG_PTR key) const;
};

