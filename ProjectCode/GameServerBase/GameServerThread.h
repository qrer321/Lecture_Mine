#pragma once
#include <thread>
#include "GameServerNameBase.h"

// �뵵 : 
// �з� :
// ÷�� : 
class GameServerThread : public GameServerNameBase
{
private: // Member Var
	std::thread m_Thread;

public: // Default
	GameServerThread();

	template <class _Fn, class... _Args, std::enable_if_t<!std::is_same_v<std::_Remove_cvref_t<_Fn>, std::thread>, int> = 0>
	explicit GameServerThread(_Fn&& _Fx, _Args&&... _Ax)
		: m_Thread(_Fx, _Ax...)
	{		
	}

	template <class _Fn, class... _Args, std::enable_if_t<!std::is_same_v<std::_Remove_cvref_t<_Fn>, std::thread>, int> = 0>
	explicit GameServerThread(std::string name, _Fn&& _Fx, _Args&&... _Ax)
		: m_Thread(_Fx, _Ax...)
	{
	}

	virtual ~GameServerThread() override;

	GameServerThread(const GameServerThread& other) = delete;
	GameServerThread(GameServerThread&& other) noexcept;

public:
	GameServerThread& operator=(const GameServerThread& other) = delete;
	GameServerThread& operator=(GameServerThread&& other) = delete;

public:
	static void ThreadNameSetting(const std::string& name);

public:
	std::thread::id ThreadId() const { return m_Thread.get_id(); }

public: // Member Function
	void join();

	template <class _Fn, class... _Args, std::enable_if_t<!std::is_same_v<std::_Remove_cvref_t<_Fn>, std::thread>, int> = 0>
	void Start(_Fn&& _Fx, _Args&&... _Ax)
	{
		m_Thread = std::thread(_Fx, _Ax...);
	}
};

