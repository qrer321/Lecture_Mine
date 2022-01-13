#pragma once

// OS 기본
#include <WinSock2.h>
#include <Windows.h> // 아래쪽에 놔야 합니다.
#include <WS2tcpip.h> // inet_pton 함수를 사용하기 위한 헤더

// 스레드
#include <thread>
#include <mutex>
#include <memory>
#include <process.h>
#include <atomic>

// 수학
#include <cmath>
#include <random>

// 자료구조
#include <list>
#include <map>
#include <stack>
#include <queue>
#include <unordered_map>
#include <unordered_set>

// 디버깅
#include <functional>
#include <cassert>
#include <crtdbg.h>

// 라이브러리
#pragma comment (lib, "ws2_32") // 서버통신용 구현이 들어있는 라이브러리