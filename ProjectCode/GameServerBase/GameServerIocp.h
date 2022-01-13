#pragma once
#include <Windows.h>
#include <mutex>
#include <functional> 
#include "GameServerObjectBase.h"
#include "GameServerThread.h"

// 용도 : 
// 분류 :
// 첨언 :

enum class IocpWaitReturnType
{
	RETURN_ERROR,
	RETURN_TIMEOUT,
	RETURN_OK,
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
	ConvertType GetConvertCompletionKey() { return reinterpret_cast<ConvertType>(CompletionKey); }

	[[nodiscard]] ULONG_PTR GetCompletionKey() const { return CompletionKey; }
	[[nodiscard]] DWORD GetNumberOfBytes() const { return NumberOfBytesTransferred; }

public:
	[[nodiscard]] size_t GetIndex() const { return Index; }

public:
	IocpWaitReturnType Wait();
};

class GameServerIocp : public GameServerObjectBase
{
private: // Member Var
	HANDLE m_IocpHandle{};
	std::mutex m_IocpLock;
	std::vector<std::shared_ptr<GameServerThread>> m_ThreadList;
	std::vector<std::shared_ptr<GameServerIocpWorker>> m_ThreadWorkerList;

public: // Default
	GameServerIocp();
	GameServerIocp(const std::function<void(std::shared_ptr<GameServerIocpWorker>)>& func, int threadCount);
	GameServerIocp(const std::function<void(std::shared_ptr<GameServerIocpWorker>)>& func, int threadCount, DWORD time);
	~GameServerIocp() override;

	GameServerIocp(const GameServerIocp& other) = delete;
	GameServerIocp(GameServerIocp&& other) noexcept;

public:
	GameServerIocp& operator=(const GameServerIocp& other) = delete;
	GameServerIocp& operator=(GameServerIocp&& other) = delete;

public:
	void Initialize(const std::function<void(std::shared_ptr<GameServerIocpWorker>)>& func, int threadCount, DWORD time);

public: // Member Function
	void AddThread(const std::function<void(std::shared_ptr<GameServerIocpWorker>)>& func, DWORD time);
	void Post(DWORD byteSize, ULONG_PTR data);
	bool Bind(HANDLE handle, ULONG_PTR key) const;

	size_t GetThreadCount() const { return m_ThreadList.size(); }
};

