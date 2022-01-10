// UDPServer.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
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
 * recvfrom(s, buf, len, flags, from, fromlen)
 * recvfrom() 함수는 UDP/IP 통신에서 소켓으로부터 데이터를 수신한다.
 *
 * @param
 * _In Socket    s       : socket() 함수로 생성된 소켓 객체
 *     char*     buf     : 수신한 데이터를 저장할 버퍼의 주소
 * _In int       len     : 읽을 데이터의 크기
 * _In int       flags   : 읽을 데이터 유형 또는 읽는 방법에 대한 option
 *     sockaddr* from    : 접속한 상대 시스템의 socket 주소 정보를 저장할 버퍼
 *                         Datagram의 통신이라 상대 시스템과 항상 연결된 것이 아니기 때문에
 *                         송신 시스템의 정보를 수신하는 이 시점에 데이터를 알 수 있으며,
 *                         sockaddr에 저장된 상대방 정보로 결과 데이터를 sendto 함수를 사용하여 전송한다.
 *     int       fromlen : sockaddr의 크기를 설정한 후에 호출한다. (INPUT)
 *                         호출 후에는 실제 할당된 sockaddr의 크기가 저장된다. (OUTPUT)
 *
 * @return
 * 성공했을 경우 실제로 수신한 데이터의 길이를 리턴한다.
 * 실패했을 경우 -1이 반환된다.
 */
int main()
{
    WSADATA wsa;
    if (0 != WSAStartup(MAKEWORD(2, 2), &wsa))
    {
        std::cout << "WSAStartup Error" << std::endl;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (INVALID_SOCKET == serverSocket)
        return 0;

    SOCKADDR_IN add = {};
    add.sin_family = AF_INET;
    add.sin_port = htons(30001);

    if (SOCKET_ERROR == inet_pton(AF_INET, "0.0.0.0", &add.sin_addr))
        return 0;

    std::cout << "server start" << std::endl;

    if (SOCKET_ERROR == bind(serverSocket, reinterpret_cast<const sockaddr*>(&add), sizeof(SOCKADDR_IN)))
        return 0;

    while (true)
    {
        SOCKADDR_IN recvAdd = {};
        int recvAddSize = sizeof(SOCKADDR_IN);
        char recvBuffer[256] = {};
        int recvSize = 0;
        
        recvSize = recvfrom(serverSocket, recvBuffer, sizeof(recvBuffer), 0, reinterpret_cast<sockaddr*>(&recvAdd), &recvAddSize);

        if (recvSize == -1)
        {
            closesocket(serverSocket);
            WSACleanup();
            assert(false);
            return 0;
        }

        std::cout << "패킷의 데이터는 : " << recvBuffer << std::endl;
    }

    closesocket(serverSocket);
    WSACleanup();
}