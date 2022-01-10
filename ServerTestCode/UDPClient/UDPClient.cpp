// UDPClient.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <iostream>
#include <conio.h>
#include <cassert>

#include <WinSock2.h>
#include <Windows.h>
#include <WS2tcpip.h>
#include <thread>
#include <mutex>
#include <memory>
#include <vector>

#pragma comment(lib, "ws2_32")

/*
 * sendto(s, buf, len, flags, to, tolen)
 * sendto() 함수는 UDP/IP 통신에서 데이터를 송신하는 함수이다
 *
 * @param
 * _In Socket			s     : socket() 함수로 생성된 소켓 객체
 *     const char*		buf   : 전송할 데이터
 * _In int				len   : 전송할 데이터의 크기
 * _In int				flags : option
 *     const sockaddr*	to    : 전송할 상대 시스템의 socket 주소 정보를 저장할 버퍼
 *     int				tolen : sockaddr의 크기
 */
int main()
{
    WSADATA wsa;
    if (0 != WSAStartup(MAKEWORD(2, 2), &wsa))
    {
        std::cout << "WSAStartup Error" << std::endl;
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (INVALID_SOCKET == clientSocket)
        return 0;

    SOCKADDR_IN sendAdd = {};
    sendAdd.sin_family = AF_INET;
    sendAdd.sin_port = htons(30001);

	if (SOCKET_ERROR == inet_pton(AF_INET, "127.0.0.1", &sendAdd.sin_addr))
	{
		return 0;
	}

	while (true)
	{
		int sendAddSize = sizeof(SOCKADDR_IN);
		char sendBuffer[256] = {};
		int sendSize = 0;

		std::cout << "패킷을 입력해주세요" << std::endl;
		std::cin >> sendBuffer;

		sendSize = sendto(clientSocket, sendBuffer, sizeof(sendBuffer), 0, 
						  reinterpret_cast<const sockaddr*>(&sendAdd), sizeof(SOCKADDR_IN));

		if (sendSize < 0)
		{
			closesocket(clientSocket);
			WSACleanup();
			assert(false);
			return 0;
		}
	}

	closesocket(clientSocket);
	WSACleanup();
}
