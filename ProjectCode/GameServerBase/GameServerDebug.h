#pragma once
#include <string>
#include <atomic>
#include <memory>

enum class LOGTYPE
{
	LOGTYPE_ERROR,
	LOGTYPE_WARNING,
	LOGTYPE_INFO,
	LOGTYPE_LASTERROR
};

class GameServerIocp;
class GameServerIocpWorker;
class GameServerDebug
{
private:
	class LogTask
	{
	public:
		LOGTYPE		m_Type;
		std::string m_LogText;
	};

private: // Member Var
	static const char*		s_TypeText[4];
	static GameServerIocp*	s_LogIocp;
	static std::atomic<int> s_LogCount;

public: // Default
	GameServerDebug() = default;
	virtual ~GameServerDebug() = 0;

	GameServerDebug(const GameServerDebug& other) = delete;
	GameServerDebug(GameServerDebug&& other) noexcept = delete;

public:
	GameServerDebug& operator=(const GameServerDebug& other) = delete;
	GameServerDebug& operator=(GameServerDebug&& other) = delete;

private:
	static void LogThread(const std::shared_ptr<GameServerIocpWorker>& worker);

public: // Member Function
	static void Initialize();
	static void Destroy();

	static void Log(LOGTYPE type, const std::string& text);
	static void LogError(const std::string & text);
	static void LogWarning(const std::string& text);
	static void LogInfo(const std::string& text);
	static void LogLastError(const std::string& text);

	static void AssertDebug();
	static void AssertDebugMsg(const std::string& msg);

	static void GetLastErrorPrint();

};

