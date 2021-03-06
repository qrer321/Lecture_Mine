#pragma once
#include <thread>
#include "GameServerNameBase.h"
#include "GameServerDebug.h"
#include "GameServerTime.h"

class GameServerThread : public GameServerNameBase
{
private: // Member Var
	std::thread									m_Thread;
	static thread_local std::string				m_ThreadName;
	static thread_local unsigned int			m_Order;
	static thread_local const std::type_info*	m_LocalDataType;
	static thread_local std::vector<char>		m_Data;

protected:
	static thread_local GameServerTime			m_Timer;

public: // Default
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

	template <class _Fn, class... _Args, std::enable_if_t<!std::is_same_v<std::_Remove_cvref_t<_Fn>, std::thread>, int> = 0>
	void Start(_Fn&& _Fx, _Args&&... _Ax)
	{
		m_Thread = std::thread(_Fx, _Ax...);
	}

	GameServerThread() = default;
	virtual ~GameServerThread() override = default;

	GameServerThread(const GameServerThread& other) = delete;
	GameServerThread(GameServerThread&& other) noexcept = delete;

	GameServerThread& operator=(const GameServerThread& other) = delete;
	GameServerThread& operator=(GameServerThread&& other) = delete;

public:
	[[nodiscard]] std::thread::id GetThreadID() const { return m_Thread.get_id(); }
	static std::string GetThreadName() { return m_ThreadName; }
	static int GetThreadOrder() { return m_Order; }

	static void SetThreadName(const std::string& name);
	static void SetThreadOrder(unsigned int order);

	void join();


	template <typename LocalDataType>
	static LocalDataType* GetLocalData()
	{
		if (m_LocalDataType != &typeid(LocalDataType))
		{
			GameServerDebug::AssertDebugMsg("Local Data Type Is Not Equal");
			return nullptr;
		}

		return reinterpret_cast<LocalDataType*>(&m_Data[0]);
	}

	template <typename LocalDataType>
	static LocalDataType* CreateThreadLocalData()
	{
		if (nullptr != m_LocalDataType)
		{
			GameServerDebug::AssertDebugMsg("Local Data Is Not Null");
			return nullptr;
		}

		m_LocalDataType = &typeid(LocalDataType);
		m_Data.resize(sizeof(LocalDataType));

		// placement new
		// ???? ?????????? ???? ???????? ???????? ???????? ???? ?? ??????
		// placement new?? ???????? ???????? ???????? ???? ????????.
		return new (&m_Data[0]) LocalDataType();
	}
};

