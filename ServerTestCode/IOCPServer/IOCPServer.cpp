// IOCPServer.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <iostream>
#include <conio.h>
#include <cassert>

#include <WinSock2.h>
#include <Windows.h>
#include <WS2tcpip.h>
#include <thread>
#include <mutex>
#include <string>
#include <vector>

#pragma comment(lib, "ws2_32")

HANDLE g_iocpHandle;
std::mutex g_iocpLock;
std::vector<std::shared_ptr<std::thread>> g_threadList;

/*
 * WSARecv(s, lpBuffers, dwBufferCount, lpNumberOfBytesRecvd, lpFlags, lpOverlapped, lpCompletionRoutine);
 * WSARecv()는 접속된 상대방 소켓으로부터 넘어온 데이터를 얻어내는 함수
 * recv 함수와 비교했을 때 overlapped 소켓에 대해서 중첩 연산을 수행할 수 있고,
 * 여러 개의 수신 버퍼를 활용할 수 있으며 flag는 입력촤 출력의 매개 변수로 사용될 수 있다.
 *
 * @param
 * SOCKET								s						: 연결 소켓을 가리키는 소켓 지정 번호
 * LPWSABUF								lpBuffers				: 데이터 버퍼를 가리키는 포인터와 버퍼의 크기 정보
 * DWORD								dwBufferCount			: WSABUF 구조체의 개수
 * LPDWORD								lpNumberOfBytesRecvd	: 데이터 입력이 완료된 경우, 읽은 데이터의 바이트 크기를 넘긴다
 * LPDWORD								lpFlags					: 함수의 호출 방식을 지정한다
 * LPWSAOVERLAPPED						lpOverlapped			: 
 * LPWSAOVERLAPPED_COMPLETION_ROUTINE	lpCompletionRoutine		: 데이터 입력이 완료되었을 때, 호출할 루틴의 포인터
 *
 * @return
 * 에러가 발생하지 않고 데이터 입력이 완료되었다면 0을 반환한다.
 * 이 경우 완료 루틴은 호출 스레드가 반응할 수 있는 상태로 호출되기 위한 준비를 갖추었다고 볼 수 있다.
 * 에러가 발생하면 SOCKET_ERROR가 반환된다.
 */

class RECVOVERDATA
{
public:
	SOCKET		m_Socket;
	WSABUF		m_Buf;
	DWORD		m_BufferCount;
	char		m_arrBuffer[1024];
	DWORD		m_NumberOfBytesRecvd;
	DWORD		m_Flags;
	OVERLAPPED	m_Overlapped;

public:
	RECVOVERDATA()
	{
		memset(this, 0, sizeof(RECVOVERDATA));

		m_Buf.len = 1024;
		m_Buf.buf = m_arrBuffer;
	}
};

void IocpThread()
{
	while (true)
	{
		DWORD NumberOfBytesTransferred;
		ULONG_PTR CompletionKey;
		LPOVERLAPPED lpOverlapped;
		GetQueuedCompletionStatus(g_iocpHandle, &NumberOfBytesTransferred, &CompletionKey, &lpOverlapped, INFINITE);

		RECVOVERDATA* recvData = reinterpret_cast<RECVOVERDATA*>(CompletionKey);
		std::cout << recvData->m_arrBuffer << std::endl;

		if (SOCKET_ERROR ==
			WSARecv(recvData->m_Socket, &recvData->m_Buf, 1, &recvData->m_NumberOfBytesRecvd,
				&recvData->m_Flags, &recvData->m_Overlapped, nullptr))
		{
			int error = GetLastError();

			if (WSA_IO_PENDING != error)
				assert(false);
		}
	}
}

int main()
{
	WSADATA wsa;
	if (0 != WSAStartup(MAKEWORD(2, 2), &wsa))
		std::cout << "WSAStartup Error" << std::endl;
	
	g_iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, NULL, 8);
	for (int i = 0; i < 8; ++i)
	{
		g_threadList.push_back(std::make_shared<std::thread>(IocpThread));
	}

	// WSA_FLAG_OVERLAPPED 플래그로 WSASocket 함수 호출 시 Overlapped 소켓으로 생성된다.
	// 이 때 어떤 수행이 바로 완료되지 못하게 되면 바로 해당 수행에 대해 실패(SOCKET_ERROR)가 리턴된다.
	// WSA_IO_PENDING 혹은 ERROR_IO_PENDING이 리턴되는 경우에는 언젠가 해당 작업이 완료될 것이므로 완료루틴을 타게 만들 수 있다.
	SOCKET serverAcceptSocket = WSASocket(AF_INET, SOCK_STREAM, 0, nullptr, 0, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET == serverAcceptSocket)
		return 0;

	/*
	 * ioctlsocket(s, cmd, argp)
	 * ioctlsocket() 함수는 소켓의 입출력 모드를 제어하는 함수
	 *
	 * @param
	 * SOCKET	s		: 소켓
	 * long		cmd		: 소켓이 수행할 명령
	 * u_long*	argp	: cmd에 대한 입출력 파라미터로 사용
	 *
	 * @desc
	 * FIONBIO
	 * argp 매개변수가 0이 아닐 경우 소켓의 non-blocking(비동기) 모드를 활성화한다.
	 *
	 * FIONREAD
	 * 네트워크 입력 버퍼에서 기다리고 있는 소켓으로부터 읽을 수 있는 데이터의 크기를 얻어내는데 사용
	 *
	 * SIOCATMARK
	 * 소켓으로부터 out-of-band 데이터가 모두 읽혀졌는지를 판단하기 위해 사용
	 */
	unsigned long mode = 1;
	int result = ioctlsocket(serverAcceptSocket, FIONBIO, &mode);
	if (SOCKET_ERROR == result)
	{
		DWORD code = GetLastError();
		assert(false);
		return 0;
	}

	SOCKADDR_IN add = {};
	add.sin_family = AF_INET;
	add.sin_port = htons(30001);

	if (SOCKET_ERROR == inet_pton(AF_INET, "0.0.0.0", &add.sin_addr))
		return 0;

	std::cout << "server start" << std::endl;

	if (SOCKET_ERROR == bind(serverAcceptSocket, reinterpret_cast<const sockaddr*>(&add), sizeof(SOCKADDR_IN)))
		return 0;

	if (SOCKET_ERROR == listen(serverAcceptSocket, SOMAXCONN))
		return 0;

	while (true)
	{
		SOCKADDR_IN userAdd = {};
		userAdd.sin_family = AF_INET;
		int len = sizeof(SOCKADDR_IN);

		SOCKET sessionSocket = accept(serverAcceptSocket, reinterpret_cast<sockaddr*>(&userAdd), &len);
		if (INVALID_SOCKET == sessionSocket)
		{
			int error = GetLastError();

			if (WSAEWOULDBLOCK == error)
			{
				Sleep(1);
				continue;
			}
			else
				assert(false);
		}

		RECVOVERDATA* newData = new RECVOVERDATA;
		newData->m_Socket = sessionSocket;

		// g_iocpHandle이 sessionSocket을 감시하라고 설정
		CreateIoCompletionPort(reinterpret_cast<HANDLE>(sessionSocket),
			g_iocpHandle, reinterpret_cast<ULONG_PTR>(newData), 0);

		if (SOCKET_ERROR == 
			WSARecv(newData->m_Socket, &newData->m_Buf, 1, &newData->m_NumberOfBytesRecvd,
			&newData->m_Flags, &newData->m_Overlapped, nullptr))
		{
			int error = GetLastError();

			if (WSA_IO_PENDING != error)
				assert(false);
		}
	}

	return 0;
}