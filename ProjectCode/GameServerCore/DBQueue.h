#pragma once
#include <GameServerBase/GameServerQueue.h>

// �뵵 : DB�� ���õ� ���� ó���ϴ� DBQueue Ŭ���� 
class DBQueue
{
private:
	static DBQueue* s_Inst;

	GameServerQueue m_TaskQueue;

public: // Default
	DBQueue() = default;
	~DBQueue();

	DBQueue(const DBQueue& other) = delete;
	DBQueue(DBQueue&& other) noexcept = delete;

	DBQueue& operator=(const DBQueue& other) = delete;
	DBQueue& operator=(DBQueue&& other) = delete;

public:
	static void Init();
	static void Queue(const std::function<void()>& callback);

	static void Destroy();
};

