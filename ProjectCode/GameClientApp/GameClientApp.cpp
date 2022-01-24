#include "PreCompile.h"
#include "tchar.h"
#include <string>

std::mutex g_Lock;
std::thread g_recvThread;

bool g_Check = true;
std::string g_recvString;

void RecvFunc(SOCKET sessionSocket)
{
	while (g_Check)
	{
		char charBuffer[1024] = {};
		wchar_t shortBuffer[1024] = {};

		int result = recv(sessionSocket, charBuffer, static_cast<int>(sizeof(charBuffer)), 0);
		if (SOCKET_ERROR == result)
			return;

		g_recvString += *charBuffer;

		{
			std::lock_guard lock(g_Lock);
			if (std::string::npos != g_recvString.find("!!"))
			{
				system("cls");
				std::cout << g_recvString << std::endl;
				g_recvString.clear();
			}
			else
			{
			 	std::cout << charBuffer << std::endl;
			}
		}
	}
}


int main()
{
	_wsetlocale(LC_ALL, L"korean");

	WSADATA wsa;

	if (0 != WSAStartup(MAKEWORD(2, 2), &wsa))
		std::cout << "WSAStartup Error" << std::endl;

	SOCKET sessionSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (INVALID_SOCKET == sessionSocket)
		return 0;

	std::cout << "IP주소를 입력해주세요" << std::endl;
	std::string Ip;
	std::string Port;
	std::cin >> Ip;

	if ("Q" == Ip
		|| "q" == Ip)
	{
		Ip = "127.0.0.1";
	}
	else if ("W" == Ip
		|| "w" == Ip)
	{
		Ip = "58.150.30.210";
	}

	SOCKADDR_IN Add = { 0, };
	Add.sin_family = AF_INET;
	Add.sin_port = htons(30001);

	if (SOCKET_ERROR == inet_pton(AF_INET, Ip.c_str(), &Add.sin_addr))
		return 0;

	if (SOCKET_ERROR == connect(sessionSocket, reinterpret_cast<const sockaddr*>(&Add), sizeof(SOCKADDR_IN)))
	{
		return 0;
	}

	std::cout << "커넥트 성공" << std::endl;

	g_recvThread = std::thread(RecvFunc, sessionSocket);

	while (true)
	{
		std::string in;

		std::cin >> in;

		if ("q" == in ||
			"Q" == in)
		{
			closesocket(sessionSocket);
			g_Check = false;
			g_recvThread.join();
			Sleep(1);
			return 0;
		}

		char buffer[1024] = {};
		memcpy_s(buffer, sizeof(buffer), in.c_str(), in.size());
		int result = send(sessionSocket, buffer, sizeof(buffer), 0);
	}

	_getch();

	return 0;
}