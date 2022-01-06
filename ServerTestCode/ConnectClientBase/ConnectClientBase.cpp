// ConnectClientBase.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <iostream>
#include <conio.h>

#include <WinSock2.h>
#include <Windows.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32")  // 서버 통신용 구현이 들어있는 라이브러리

int main()
{
    /*
    * struct WSADATA
    * WORD				wVersion								: 버전
    * WORD				wHighVersion							: 사용할 수 있는 상위 버전으로 wVersion과 일치한다
    * unsigned short	iMaxSockets								: 최대 소켓
    * unsigned short	iMaxUdpDg								: 데이터 그램의 최대 크기
    * char FAR*		    lpVenderInfo							: 벤더 정보(큰 의미 없음)
    * char				szDescription[WSADESCRIPTION_LEN + 1]	: 윈속 설명
    * char				szSystemStatus[WSASYS_STATUS_LEN + 1]	: 상태 문자열
    */
    WSADATA wsa;

    /*
     * 윈도우에 프로그램이 소켓통신을 하겠다고 미리 알려주는 함수
     *
     * MAKEWORD(2, 2) : 소켓 통신 방식 버전을 알려준다.
     * 대부분 2.2 버전이기 때문에 아래와 같이 입력해주면 된다.
     */
    if (0 != WSAStartup(MAKEWORD(2, 2), &wsa))
    {
        std::cout << "WSAStartup Error" << std::endl;
    }

    /*
     * socket( )
     *
     * @param
     * int af(domain)   : 어떤 영역에서 통신할 것이지에 대한 영역을 지정한다. 프로토콜 family를 지정
     *                    AF_INET(IPv4) / AF_INET6(IPv6) / AF_UNIX(local to host (pipes, portals))
     * int type         : 어떤 타입의 프로토콜을 사용할 것인지에 대해 설정
     *                    SOCK_STREAM(TCP) / SOCK_DGRAM(UDP) / SOCK_RAW(사용자 정의)
     * int protocol     : 어떤 프로토콜의 값을 결정하는 것
     *                    0 / IPPROTO_TCP(TCP일 때) / IPPROTO_UDP(UDP일 때)
     * @return
     * SOCKET(UINT_PTR) : 해당 소켓을 가리키는 소켓 디스크립터(socket descriptor) 반환
     *                    -1(소켓 생성 실패) / 0 이상의 값(socket descriptor 반환)
     */
    SOCKET ServerConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == ServerConnectSocket)
        return 0;

    // 클라이언트는 bind 할 필요가 없다.

    std::cout << "접속 준비 키를 누르면 접속합니다" << std::endl;
    _getch();

    /*
     * 소켓통신을 위한 주소정보를 담는 구조체
     * ADDRESS_FAMILY sin_family : 주소체계
     * USHORT   sin_port         : 16비트 포트 번호, network byte order
     * IN_ADDR  sin_addr         : 32비트 IP 주소
     * CHAR     sin_zero[8]      : 전체 크기를 16비트로 맞추기 위한 dummy
     *
     * sin_port : 포트 번호를 가지며 2bytes이다. 즉 포트번호는 0 ~ 65535의 범위를 갖는 숫자 값이다.
     *            이 변수에 저장되는 값은 network byte order이어야 한다.
     *            추가설명. ip주소로 서버가 존재하는 컴퓨터까지는 올 수 있다.
     *            여러 개의 프로그램들이 통신을 하고 있을 때 해당 프로그램 중 어느 프로그램에 패킷을 보내야 하는지 결정
     *            특정 번호를 정해서 패킷이 도착할 위치를 정할 수 있다.
     *
     * sin_addr : 호스트 IP 주소이다. 이 변수에는 INADDR_로 시작하는 값이 저장되어야 한다.
     *            혹은 inet_pton() 함수와 같은 라이브러리가 제공하는 함수의 반환값이 저장되어야 한다.
     *
     * sin_zero : 8 bytes dummy data이다. 반드시 모두 0으로 채워져 있어야 한다.
     *            sockaddr_in이 sin_zero를 제외한 크기가 8 bytes이므로, zero를 합쳐 총합 16bytes이다.
     *            struct sockaddr 구조체와 크기를 일치시키려는 목적으로 사용한다.
     */
    SOCKADDR_IN add = {};
    add.sin_family = AF_INET;               // IP버전 4로 주소체계를 잡는다
    add.sin_port = htons(30001);    // 30001 포트로 빅엔디안으로 변환하여 설정

    /*
     * inet_pton
     * @param
     * _In  INT   Family        : address family를 지정한다. pszAddrString 문자열이 IPv4 / IPv6 주소를 나타내는지 함수에 알린다.
     * _In  PCSTR pszAddrString : 문자열 형태의 IP주소를 넣는다.
     * _Out PVOID pAddrBuf      : 입력된 IP주소를 binary 형태로 변환 후 복사된 메모리의 포인터이다.
     */
    if (SOCKET_ERROR == inet_pton(AF_INET, "61.98.125.214", &add.sin_addr))
        return 0;

    /*
     * connect(s, name, namelen)
     * connect() 함수는 지정된 소켓으로 연결을 시도한다.
     *
     * @param
     * _In SOCKET          s        : 연결 하려는(연결되어 있지 않은) 소켓 객체
     *     const sockaddr* name     : 연결정보를 담고 있는 sockaddr 구조체의 포인터
     * _In int             namelen  : sockaddr 구조체 포인터가 가리키는 데이터의 크기
     */
    if (SOCKET_ERROR == connect(ServerConnectSocket, reinterpret_cast<const sockaddr*>(&add), sizeof(SOCKADDR_IN)))
        return 0;

    std::cout << "접속에 성공했습니다" << std::endl;

    _getch();
}