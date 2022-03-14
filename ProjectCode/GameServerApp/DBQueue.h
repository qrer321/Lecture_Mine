#pragma once
#include <GameServerBase/GameServerQueue.h>

class TestClass
{
public:
	int value{};

public:
	TestClass() = default;
	virtual ~TestClass() = default;
};

// 용도 : DB에 관련된 일을 처리하는 DBQueue 클래스 
class DBQueue
{
private:
	static GameServerQueue s_TaskQueue;

public: // Default
	DBQueue() = default;
	~DBQueue() = default;

	DBQueue(const DBQueue& other) = delete;
	DBQueue(DBQueue&& other) noexcept = delete;

	DBQueue& operator=(const DBQueue& other) = delete;
	DBQueue& operator=(DBQueue&& other) = delete;

public:
	static void Init();
	static void Queue(const std::function<void()>& callback);

	static void Destroy();
};

