#pragma once

#include <WinSock2.h>
#include <Windows.h> // �Ʒ��ʿ� ���� �մϴ�.
#include <WS2tcpip.h> // inet_pton �Լ��� ����ϱ� ���� ���
#include <conio.h>
#include <cassert>
#include <functional>
#include <thread>
#include <memory>
#include <mutex>
#include <vector>

#pragma comment (lib, "ws2_32") // ������ſ� ������ ����ִ� ���̺귯��