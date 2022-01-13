#pragma once

#include "GameServerNameBase.h"
#include "GameServerIocp.h"

// 용도 : 
// 분류 :
// 첨언 : IOCP를 한번 감싸서 기본 규칙을 정의한 Queue
//		  관리되는 IOCP라고 본다.
class GameServerQueue : public GameServerNameBase
{
public:
	enum class WORK_TYPE : char
	{
		Default,	// 기본형, Post n번에 n번 리턴
		Extension	// 확장형, Post n번에 운영체제가 알아서 횟수를 정해 리턴
	};

private:
	// 일을 시키기
	struct PostTask
	{
		std::function<void()> task;
	};

	// 비동기 파일 입출력
	struct OverlappedTask
	{
		// BOOL			: 성공/실패
		// DWORD		: 받은 패킷이나 처리한 데이터의 길이
		// LPOVERLAPPED : 비동기 입출력의 정보를 담고있는 OVERLAPPED 포인터
		std::function<void(BOOL, DWORD, LPOVERLAPPED)> task;
	};

	enum class QUEUE_RETURN : char
	{
		OK,
		DESTROY
	};

	enum class WORK_MESSAGE_TYPE : char
	{
		MSG_POST = -2,
		MSG_DESTROY = -1

		// 0 이상인 경우 서버 메시지
	};

private:
	GameServerIocp m_Iocp;
	std::function<QUEUE_RETURN(std::shared_ptr<GameServerIocpWorker> work)> m_WorkFunction;

public: // Default
	GameServerQueue();
	GameServerQueue(WORK_TYPE type, int threadCount, const std::string& threadName = "");
	~GameServerQueue() override;

	GameServerQueue(const GameServerQueue& other) = delete;
	GameServerQueue(GameServerQueue&& other) noexcept;

public:
	GameServerQueue& operator=(const GameServerQueue& other) = delete;
	GameServerQueue& operator=(GameServerQueue&& other) = delete;

private:
	static void QueueFunction(const std::shared_ptr<GameServerIocpWorker>& work, GameServerQueue* self, const std::string& threadName);

	void Run(const std::shared_ptr<GameServerIocpWorker>& work);
	QUEUE_RETURN Work(const std::shared_ptr<GameServerIocpWorker>& work);
	QUEUE_RETURN WorkDefault(const std::shared_ptr<GameServerIocpWorker>& work);		// 한번에 하나씩
	QUEUE_RETURN WorkExtension(const std::shared_ptr<GameServerIocpWorker>& work);		// 한번에 여러개

	void SetWorkType(WORK_TYPE type);

public: // Member Function
	void EnQueue(const std::function<void()>& callback);
	void NetworkBind(SOCKET socket, const std::function<void(BOOL, DWORD, LPOVERLAPPED)>& callback);

	void Destroy();
};

