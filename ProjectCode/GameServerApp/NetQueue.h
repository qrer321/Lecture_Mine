#pragma once
#include <GameServerBase/GameServerQueue.h>

class NetQueue
{
private:
	static GameServerQueue s_TaskQueue;

public: // Default
	NetQueue() = default;
	~NetQueue() = default;

	NetQueue(const NetQueue& other) = delete;
	NetQueue(NetQueue&& other) noexcept = delete;

	NetQueue& operator=(const NetQueue& other) = delete;
	NetQueue& operator=(NetQueue&& other) = delete;

public:
	static GameServerQueue& GetQueue() { return s_TaskQueue; }

public:
	static void Init();
	static void Queue(const std::function<void()>& callback);

	static void Destroy();
};

