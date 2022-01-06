// AcceptServerBase.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//
#include <iostream>
#include <conio.h>  // console input, output header

#include <WinSock2.h>
#include <Windows.h>
#include <WS2tcpip.h>// inet_pton 함수를 사용하기 위한 헤더

#pragma comment(lib, "ws2_32")  // 서버 통신용 구현이 들어있는 라이브러리

int main()
{
    /*
     * struct WSADATA
     * WORD				wVersion								: 버전
     * WORD				wHighVersion							: 사용할 수 있는 상위 버전으로 wVersion과 일치한다
     * unsigned short   iMaxSockets								: 최대 소켓
     * unsigned short	iMaxUdpDg								: 데이터 그램의 최대 크기
     * char FAR*	    lpVenderInfo							: 벤더 정보(큰 의미 없음)
     * char				szDescription[WSADESCRIPTION_LEN + 1]	: 윈속 설명
     * char				szSystemStatus[WSASYS_STATUS_LEN + 1]	: 상태 문자열
     */
    WSADATA wsa;

    /*
     * WSAStartup(wVersionRequired, lpWSAData)
     * 윈도우에 프로그램이 소켓통신을 하겠다고 미리 알려주는 함수
     *
     * @param
     * _In  WORD      wVersionRequired : 애플리케이션이 사용할 수 있는 WinSock API의 최상위 버전
     *                                   상위 바이트가 마이너 버전, 하위 바이트가 메이저 버전
     * _Out LPWSADATA lpWSAData        : WSADATA 타입의 구조체에 대한 포인터
     *
     * MAKEWORD(2, 2) : 소켓 통신 방식 버전을 알려준다.
     * 대부분 2.2 버전이기 때문에 아래와 같이 입력해주면 된다.
     */
    if (0 != WSAStartup(MAKEWORD(2, 2), &wsa))
    {
        std::cout << "WSAStartup Error" << std::endl;
    }

    /*
     * SOCKADDR_IN
     * 소켓통신을 위한 주소정보를 담는 구조체
     *
     * @param
     * ADDRESS_FAMILY   sin_family  : 주소체계
     * USHORT           sin_port    : 16비트 포트 번호, network byte order
     * IN_ADDR          sin_addr    : 32비트 IP 주소
     * CHAR             sin_zero[8] : 전체 크기를 16비트로 맞추기 위한 dummy
     *
     * @desc
     * sin_port : 포트 번호를 가지며 2bytes이다. 즉 포트번호는 0 ~ 65535의 범위를 갖는 숫자 값이다.
     *            이 변수에 저장되는 값은 network byte order이어야 한다.
     *            ip주소로 서버가 존재하는 컴퓨터까지는 올 수 있다.
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
     * inet_pton(Family, pszAddrString, pAddrBuf)
     * inet_pton() 함수는 사람이 알아보기 쉬운 텍스트 형태의 IPv4와 IPv6 주소를 binary 형태로 변환하는 함수
     *
     * @param
     * _In  INT   Family        : address family를 지정한다. pszAddrString 문자열이 IPv4 / IPv6 주소를 나타내는지 함수에 알린다.
     * _In  PCSTR pszAddrString : 문자열 형태의 IP주소를 넣는다.
     * _Out PVOID pAddrBuf      : 입력된 IP주소를 binary 형태로 변환 후 복사된 메모리의 포인터이다.
     *
     * @return
     * 성공 시 1을 반환한다.
     * 즉, 1이 return되면 네트워크 주소가 성공적으로 변환되었음을 알린다.
     *
     * 0이 반환되는 경우 pszAddrString의 문자열이 나타내는 네트워크 주소가,
     * Family에 명시된 address family의 유효한 값이 아닌 경우이다.
     *
     * -1이 반환되는 경우 Family가 적절한 address family 값이 아닌 경우이다.
     * 이 때는 errno가 EAFNOSUPPORT 값으로 설정된다.
     *
     * @desc
     * Family 매개변수의 address family가 가리키는 네트워크 주소 구조체에
     * pszAddrString의 문자열의 값을 변환한 후,
     * 이 네트워크 주소 구조체를 pAddrBuf(sin_addr)에 복사한다.
     *
     * Family는 반드시 AF_INET 혹은 AF_INET6, 둘 중 하나여야 한다.
     * pAddrBuf는 network byte order로 작성된다.
     *
     * inet_aton() 및 inet_addr()와는 달리, inet_pton은 IPv4 / IPv6 주소를 모두 지원한다.
     */
    if (SOCKET_ERROR == inet_pton(AF_INET, "0.0.0.0", &add.sin_addr))
        return 0;

    std::cout << "server start" << std::endl;

    /*
     * socket(af, type, protocol)
     *
     * @param
     * int af(domain)   : 주소영역을 정의하기 위해서 사용한다.
     *                    AF_INET   : IPv4 주소영역
     *                    AF_INET6  : IPv6 주소영역
     * int type         : 소켓의 통신 타입을 지정하기 위해서 사용한다.
     *                    SOCK_STREAM(TCP)      : 연결지향, 양방향의 TCP/IP 기반의 통신을 위해서 사용된다
     *                    SOCK_DGRAM(UDP)       : UDP/IP 기반의 통신을 위해 사용한다.
     *                    SOCK_RAW(사용자 정의)  : IP 헤더를 직접 제어하기 위한 목적으로 사용한다.
     * int protocol     : 호스트간 통신에 사용할 프로토콜을 결정하기 위해서 사용한다.
     *                    IPPROTO_TCP : TCP를 사용한다.
     *                    IPPROTO_UDP : UDP를 사용한다.
     *
     * @return
     * SOCKET(UINT_PTR) : 해당 소켓을 가리키는 소켓 디스크립터(socket descriptor) 반환
     *                    -1(소켓 생성 실패) / 0 이상의 값(socket descriptor 반환)
     */
    SOCKET ServerAcceptSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == ServerAcceptSocket)
        return 0;

    /*
     * bind(s, name, namelen)
     * 소켓에 주소, 프로토콜, 포트를 할당한다.
     *
     * @param
     * _In Socket          s        : socket() 함수로 생성된 소켓 객체
     * _In const sockaddr* name     : 소켓 객체에 부여할 주소정보를 포함한 구조체
     * _In int             namelen  : name의 데이터 길이. byte 단위
     *
     * @return
     * 성공했을 경우 0을 리턴한다.
     * 실패했을 경우 SOCKET_ERROR을 리턴하고 error code를 설정한다.
     * WSAGetLastError() 함수를 호출해 error code 값을 알 수 있다.
     */
    if (SOCKET_ERROR == bind(ServerAcceptSocket, reinterpret_cast<const sockaddr*>(&add), sizeof(SOCKADDR_IN)))
        return 0;

    /*
     * listen(s, backlog)
     * listen 함수는 연결요청 소켓이 대기하는 연결 대기열을 생성한다.
     *
     * @param
     * _In Socket   s       : socket() 함수로 생성된 SOCKET 객체
     * _In int      backlog : 연결 대기열의 크기. 어느 정도의 크기가 효율적인지는 정해진바가 없다.
     *                        네트워크 상태와 서비스 종류에 따라서 달라진다. 보통 5 정도를 사용
     *                        WinSock2 부터는 SOMAXCONN라는 상수 값을 사용한다.
     *                        SOMAXCONN을 지정하면 소켓 서비스 제공자가 알아서 backlog 값을 설정한다.
     *
     * @desc
     * 클라이언트가 connect() 함수를 호출해서 연결요청을 하면,
     * 서버는 socket() 함수로 생성한 듣기 소켓으로 연결요청을 생성한다.
     *
     * 생성된 연결요청은 listen 대기열에 들어가며 listen 대기열의 연결요청을 꺼내는 함수는 accept() 함수이다.
     * accept() 함수는 listen 대기열에서 가장 오래된 요청을 꺼내와서 연결 소켓을 생성한다.
     * 클라이언트와 서버는 연결 소켓을 이용해서 통신한다.
     */
    if (SOCKET_ERROR == listen(ServerAcceptSocket, SOMAXCONN))
        return 0;


    std::cout << "서버 접속 함수 실행" << std::endl;

    SOCKADDR_IN userAdd = {};
    int len = sizeof(SOCKADDR_IN);

    /*
     * accept(s, addr, addrlen)
     * accept() 함수는 연결 대기열의 가장 앞에 있는(먼저 들어온)
     * 클라이언트 연결 요청을 가져와 "연결 소켓"을 만든다.
     *
     * @param
     * _In SOCKET    s       : 클라이언트의 연결을 기다리는 소켓 객체
     *                         요청을 듣는다고 하여 흔히 '듣기 소켓'이라고 한다.
     *     sockaddr* addr    : 클라이언트 연결을 가져오면 이 매개 변수에 클라이언트 주소 정보를 복사해 넘긴다.
     *     int*      addrlen : 두 번째 매개변수 자료구조의 크기
     *
     * @return
     * 실패하면 INVALID_SOCKET을 반환한다.
     * 성공하면 소켓 지정 번호인 SOCKET 형의 값을 반환한다.
     * 이 소켓 지정 번호는 클라이언트 연결 소켓을 가리킨다.
     *
     * 이 소켓을 이용해서 클라이언트와 데이터 통신을 할 수 있다.
     * 클라이언트와 연결된 소켓이라고 해서 흔히 '연결 소켓'이라고 부른다.
     */
    SOCKET sessionSocket = accept(ServerAcceptSocket, reinterpret_cast<sockaddr*>(&userAdd), &len);
    if (INVALID_SOCKET == sessionSocket)
        return 0;

    std::cout << "패킷 보내기 전 대기" << std::endl;
    _getch();

    int result = send(sessionSocket, "hi", sizeof("hi"), 0);
    if (SOCKET_ERROR == result)
        return 0;

    std::cout << "접속자가 있습니다" << std::endl;

    _getch();

    WSACleanup();
}