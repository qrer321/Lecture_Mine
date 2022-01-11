#pragma once

#include <WinSock2.h>
#include <Windows.h> // 아래쪽에 놔야 합니다.
#include <WS2tcpip.h> // inet_pton 함수를 사용하기 위한 헤더
#include <conio.h>
#include <cassert>
#include <functional>
#include <thread>
#include <memory>
#include <mutex>
#include <vector>

#pragma comment (lib, "ws2_32") // 서버통신용 구현이 들어있는 라이브러리