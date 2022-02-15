#include "PreCompile.h"
#include "ServerHelper.h"

void ServerHelper::ServerStartup()
{
	static bool start_check = false;
	if (true == start_check)
		return;

	WSAData wsa = {};
	if (SOCKET_ERROR == WSAStartup(MAKEWORD(2, 2), &wsa))
	{
		GameServerDebug::GetLastErrorPrint();
		return;
	}

	start_check = true;
}