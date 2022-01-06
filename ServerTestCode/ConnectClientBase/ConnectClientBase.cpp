// ConnectClientBase.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <iostream>
#include <conio.h>

#include <WinSock2.h>
#include <Windows.h>
#include <WS2tcpip.h>
#include <string>

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
    SOCKET sessionSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == sessionSocket)
        return 0;

    // 클라이언트는 bind 할 필요가 없다.

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
    add.sin_family = AF_INET;        // IP버전 4로 주소체계를 잡는다
    add.sin_port = htons(port_s);    // 30001 포트로 빅엔디안으로 변환하여 설정

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

    /*
     * connect(s, name, namelen)
     * connect() 함수는 지정된 소켓으로 연결을 시도한다.
     *
     * @param
     * _In SOCKET          s        : 연결 하려는(연결되어 있지 않은) 소켓 객체
     *     const sockaddr* name     : 연결정보를 담고 있는 sockaddr 구조체의 포인터
     * _In int             namelen  : sockaddr 구조체 포인터가 가리키는 데이터의 크기
     */
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
    
    std::cout << "정상적으로 접속하였습니다" << std::endl;
    std::cout << "서버의 입력을 대기하고 있습니다" << std::endl << std::endl;
        

    char buffer[1024] = {};
    int result = recv(sessionSocket, buffer, sizeof(buffer), 0);
    if (SOCKET_ERROR == result)
        return 0;

    std::cout << "접속에 성공했습니다" << std::endl;

    _getch();

    return 0;
}