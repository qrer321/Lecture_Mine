#include "PreCompile.h"
#include "ServerHelper.h"

void ServerHelper::StartEngineStartUp()
{
	static bool start_check = false;
	if (true == start_check)
		return;

	WSAData wsa = {};
	int error = WSAStartup(MAKEWORD(2, 2), &wsa);
	if (SOCKET_ERROR == error)
	{
		GameServerDebug::GetLastErrorPrint();
		return;
	}

	start_check = true;
}

ServerHelper::ServerHelper()
= default;

ServerHelper::~ServerHelper()
= default;

ServerHelper::ServerHelper(ServerHelper&& other) noexcept
{

}