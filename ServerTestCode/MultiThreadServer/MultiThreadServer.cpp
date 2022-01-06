// AcceptServerBase.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//
#include <iostream>
#include <conio.h>

#include <WinSock2.h>
#include <Windows.h>
#include <WS2tcpip.h>
#include <thread>
#include <mutex>
#include <memory>
#include <vector>

#pragma comment(lib, "ws2_32")

std::mutex                                  g_sessionLock;
std::vector<std::shared_ptr<std::thread>>   g_threadList;
std::vector<SOCKET>                         g_sessionSockets;

void UserThreadFunc(SOCKET sessionSocket)
{
	while (true)
	{
        char buffer[1024] = {};

        recv(sessionSocket, buffer, sizeof(buffer), 0);

        std::cout << buffer << std::endl;

        g_sessionLock.lock();

        for (size_t i = 0; i < g_sessionSockets.size(); ++i)
        {
            if (g_sessionSockets[i] == sessionSocket)
                continue;

            send(g_sessionSockets[i], buffer, sizeof(buffer), 0);
        }

        g_sessionLock.unlock();
	}
}

int main()
{
    WSADATA wsa;
    if (0 != WSAStartup(MAKEWORD(2, 2), &wsa))
    {
        std::cout << "WSAStartup Error" << std::endl;
    }
    
    SOCKADDR_IN add = {};
    add.sin_family = AF_INET;
    add.sin_port = htons(30001);

    if (SOCKET_ERROR == inet_pton(AF_INET, "0.0.0.0", &add.sin_addr))
        return 0;

    std::cout << "server start" << std::endl;
    
    SOCKET ServerAcceptSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == ServerAcceptSocket)
        return 0;
    
    if (SOCKET_ERROR == bind(ServerAcceptSocket, reinterpret_cast<const sockaddr*>(&add), sizeof(SOCKADDR_IN)))
        return 0;
    
    if (SOCKET_ERROR == listen(ServerAcceptSocket, SOMAXCONN))
        return 0;

    while (true)
    {
        std::cout << "서버 접속 함수 실행" << std::endl;

        SOCKADDR_IN userAdd = {};
        int len = sizeof(SOCKADDR_IN);

        SOCKET sessionSocket = accept(ServerAcceptSocket, reinterpret_cast<sockaddr*>(&userAdd), &len);
        if (INVALID_SOCKET == sessionSocket)
            return 0;

        g_sessionLock.lock();

        g_threadList.push_back(std::make_shared<std::thread>(UserThreadFunc, sessionSocket));
        g_sessionSockets.push_back(sessionSocket);

        g_sessionLock.unlock();
    }

    _getch();

    WSACleanup();
}