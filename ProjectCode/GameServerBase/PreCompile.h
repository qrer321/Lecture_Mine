#pragma once

// OS �⺻
#include <WinSock2.h>
#include <Windows.h> // �Ʒ��ʿ� ���� �մϴ�.
#include <WS2tcpip.h> // inet_pton �Լ��� ����ϱ� ���� ���
#include <MSWSock.h>

// ������
#include <thread>
#include <mutex>
#include <memory>
#include <process.h>
#include <atomic>

// ����
#include <cmath>
#include <random>

// �ڷᱸ��
#include <list>
#include <map>
#include <stack>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <string>
#include <sstream>
#include <deque>

// �����
#include <functional>
#include <cassert>
#include <crtdbg.h>

// IO
#include <filesystem>

// ���̺귯��
#pragma comment (lib, "ws2_32") // ������ſ� ������ ����ִ� ���̺귯��
#pragma comment (lib, "mswsock")