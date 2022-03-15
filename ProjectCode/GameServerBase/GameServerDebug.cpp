#include "PreCompile.h"
#include "GameServerDebug.h"
#include "GameServerIocp.h"
#include <Windows.h>
#include <iostream>

const char*			GameServerDebug::s_TypeText[4] = { "ERROR	: ", "WARNING	: ", "INFO	: ", "LASTERROR	: "};
GameServerIocp*		GameServerDebug::s_LogIocp = new GameServerIocp;
std::atomic<int>	GameServerDebug::s_LogCount;

GameServerDebug::~GameServerDebug() = default;

void GameServerDebug::LogThread(const std::shared_ptr<GameServerIocpWorker>& worker)
{
	while (true)
	{
		worker->Wait();

		if (-1 == static_cast<int>(worker->GetNumberOfBytes()))
			return;

		++GameServerDebug::s_LogCount;

		std::unique_ptr<LogTask> logPtr = std::unique_ptr<LogTask>(worker->GetConvertCompletionKey<LogTask*>());
		std::cout << s_TypeText[static_cast<int>(logPtr->m_Type)] << logPtr->m_LogText << std::endl;
	}
}

void GameServerDebug::Initialize()
{
	s_LogIocp->Initialize(LogThread, 1, INFINITE);
}

void GameServerDebug::Destroy()
{
	s_LogIocp->Post(-1, 0);
	delete s_LogIocp;
	Sleep(1);
}

void GameServerDebug::Log(LOGTYPE type, const std::string& text)
{
	std::unique_ptr<LogTask> logPtr = std::make_unique<LogTask>();

	logPtr->m_Type = type;
	logPtr->m_LogText = text;
	s_LogIocp->Post(0, reinterpret_cast<ULONG_PTR>(logPtr.get()));

	logPtr.release();
}

void GameServerDebug::LogError(const std::string& text)
{
	Log(LOGTYPE::LOGTYPE_ERROR, text);
}

void GameServerDebug::LogWarning(const std::string& text)
{
	Log(LOGTYPE::LOGTYPE_WARNING, text);
}

void GameServerDebug::LogInfo(const std::string& text)
{
	Log(LOGTYPE::LOGTYPE_INFO, text);
}

void GameServerDebug::LogLastError(const std::string& text)
{
	Log(LOGTYPE::LOGTYPE_LASTERROR, text);
}

void GameServerDebug::AssertDebug()
{
	assert(false);
}

void GameServerDebug::AssertDebugMsg(const std::string& msg)
{
	MessageBoxA(nullptr, msg.c_str(), "∞Ê∞Ì√¢", MB_OK);
	AssertDebug();
}

void GameServerDebug::GetLastErrorPrint()
{
	DWORD error = WSAGetLastError();
	char* message = nullptr;

	FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
		nullptr,
		error,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<char*>(&message),
		0,
		nullptr);

	if (nullptr != message)
	{
		std::string text = "Code : ";
		text += std::to_string(error);
		text += " Message : ";
		text += message;

		LogLastError(text);
		LocalFree(message);
	}
}