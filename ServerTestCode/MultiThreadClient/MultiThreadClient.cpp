// ConnectClientBase.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <iostream>
#include <conio.h>

#include <WinSock2.h>
#include <Windows.h>
#include <WS2tcpip.h>
#include <thread>
#include <mutex>

#pragma comment(lib, "ws2_32")

/*
 * send(s, buf, len, flags)
 * send() 함수를 통해 소켓으로 연결되어 있는 다른 프로세스에게 데이터를 보낼 수 있다.
 *
 * @param
 * _In SOCKET       s       : 데이터를 보낼 연결상태의 소켓
 *     const char*  buf     : 전송할 데이터를 담고있는 데이터 버퍼의 포인터
 * _In int          len     : 데이터 버퍼의 byte 사이즈
 * _In int          flags   : 특별한 옵션을 주기 위한 플래그. 0을 주어 생략 가능
 *
 * @return
 * 오류가 없다면 보냈던 데이터의 byte 사이즈를 반환한다.
 * 오류가 발생한다면 SOCKET_ERROR라는 오류 코드를 반환한다.
 *
 *
 * recv(s, buf, len, flags)
 * recv() 함수를 통해 다른 프로세스로부터 온 데이터를 수신할 수 있다.
 *
 * @param
 * _In SOCKET   s       : 데이터를 수신할 연결상태의 소켓
 *     char*    buf     : 수신된 데이터를 담을 데이터 버퍼의 포인터
 * _In int      len     : 데이터 버퍼의 byte 사이즈
 * _In int      flags   : 0을 주어 생략 가능
 *
 * @return
 * send() 함수와 동일하게
 * 정상적으로 수신되었을 때 받은 데이터의 byte 사이즈를 반환한다.
 * 오류가 발생한다면 SOCKET_ERROR라는 오류 코드를 반환한다.
 */

std::mutex g_sessionLock;
std::thread g_recvThread;

void RecvFunc(SOCKET sessionSocket)
{
	while (true)
	{
        char buffer[1024] = {};

        recv(sessionSocket, buffer, sizeof(buffer), 0);

        std::cout << buffer << std::endl;
	}
}

int main()
{
    WSADATA wsa;
    if (0 != WSAStartup(MAKEWORD(2, 2), &wsa))
    {
        std::cout << "WSAStartup Error" << std::endl;
    }
    
    SOCKET sessionSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == sessionSocket)
        return 0;
    
    std::cout << "IP 주소를 입력해주세요" << std::endl;
    std::string ip;
    std::string port;
    std::cin >> ip;

    if ("q" == ip ||
        "Q" == ip)
    {
        ip = "127.0.0.1";
    }
    
    SOCKADDR_IN add = {};
    add.sin_family = AF_INET;
    add.sin_port = htons(30001);
    
    if (SOCKET_ERROR == inet_pton(AF_INET, ip.c_str(), &add.sin_addr))
        return 0;
    
    if (SOCKET_ERROR == connect(sessionSocket, reinterpret_cast<const sockaddr*>(&add), sizeof(SOCKADDR_IN)))
        return 0;

    g_recvThread = std::thread(RecvFunc, sessionSocket);
    while (true)
    {
        std::string in;

        std::cin >> in;

        char buffer[1024] = {};
        memcpy_s(buffer, sizeof(buffer), in.c_str(), in.size());
        send(sessionSocket, buffer, sizeof(buffer), 0);
    }

    _getch();
}