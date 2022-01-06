// ConnectClientBase.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <iostream>
#include <conio.h>

#include <WinSock2.h>
#include <Windows.h>
#include <WS2tcpip.h>
#include <thread>
#include <mutex>
#include <string>

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
    
    std::cout << "IP 주소를 입력해주세요  ";
    std::cout << "(q를 입력하시면 127.0.0.1:30001로 접속합니다)" << std::endl;
    std::string ip;
    std::string port;
    std::cin >> ip;
    system("cls");

    if ("q" == ip ||
        "Q" == ip)
    {
        ip = "127.0.0.1";
        port = "30001";
    }

    size_t portPos = ip.find(':');
    if (port.empty() && std::string::npos == portPos)
    {
        std::cout << "포트 번호를 입력해주세요" << std::endl;
        std::cin >> port;
        system("cls");
    }
    else if (port.empty() && std::string::npos != portPos)
    {
        port = ip.substr(portPos + 1, ip.length() - portPos);
        ip = ip.substr(0, portPos);
    }


    u_short port_s = 0;
    try
    {
        port_s = static_cast<u_short>(std::stoi(port));
    }
    catch (...)
    {
        std::cout << "잘못된 포트 번호를 입력하였습니다" << std::endl;

        std::cout << std::endl << std::endl;
        std::cout << "입력한 포트 번호 : " << port << std::endl;
        system("pause");
        return 0;
    }
    
    SOCKADDR_IN add = {};
    add.sin_family = AF_INET;
    add.sin_port = htons(30001);
    
    INT pton_return_value = inet_pton(AF_INET, ip.c_str(), &add.sin_addr);
    if (SOCKET_ERROR == pton_return_value)
        return 0;

    if (0 == pton_return_value)
    {
        std::cout << "입력한 IP가 IPv4 형태의 주소가 아닙니다" << std::endl;

        std::cout << std::endl << std::endl;
        std::cout << "입력한 IP : " << ip << std::endl;
        system("pause");

        return 0;
    }

    std::cout << "IP 주소     : " << ip << std::endl;
	std::cout << "port 번호   : " << port << std::endl << std::endl << std::endl;

	if (SOCKET_ERROR == connect(sessionSocket, reinterpret_cast<const sockaddr*>(&add), sizeof(SOCKADDR_IN)))
	{
		int errorValue = WSAGetLastError();
		if (WSAECONNREFUSED == errorValue)
		{
			std::cout << "연결이 거부되었습니다" << std::endl;
			std::cout << "포트 번호를 다시 확인해주세요" << std::endl;
			system("pause");
		}
        else if (WSAEADDRNOTAVAIL == errorValue)
        {
            std::cout << "원격 주소가 유효한 주소가 아닙니다" << std::endl;
            std::cout << "IP주소를 다시 확인해주세요" << std::endl;
            system("pause");
        }
		else
		{
			std::cout << "알 수 없는 오류가 발생하였습니다" << std::endl;
			std::cout << "오류 코드를 확인해주세요" << std::endl;
			std::cout << std::endl << std::endl;
			std::cout << errorValue << std::endl;
			system("pause");
		}

		return 0;
	}

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