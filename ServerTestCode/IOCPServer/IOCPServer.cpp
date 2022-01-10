// IOCPServer.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <iostream>
#include <conio.h>

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

void IocpThread()
{
	while (true)
	{
		DWORD NumberOfBytesTransferred;
		ULONG_PTR CompletionKey;
		LPOVERLAPPED lpOverlapped;
		GetQueuedCompletionStatus(g_iocpHandle, &NumberOfBytesTransferred, &CompletionKey, &lpOverlapped, INFINITE);

		int a = 0;
	}
}

int main()
{
	g_iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, NULL, 8);
	for (int i = 0; i < 8; ++i)
	{
		g_threadList.push_back(std::make_shared<std::thread>(IocpThread));
	}

	_getch();

	return 0;
}