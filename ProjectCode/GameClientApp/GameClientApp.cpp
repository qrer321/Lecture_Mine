#include "PreCompile.h"

int main()
{
	WSADATA wsa;

	if (0 != WSAStartup(MAKEWORD(2, 2), &wsa))
		std::cout << "WSAStartup Error" << std::endl;

	SOCKET SessionSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (INVALID_SOCKET == SessionSocket)
		return 0;

	std::cout << "IP�ּҸ� �Է����ּ���" << std::endl;
	std::string Ip;
	std::string Port;
	std::cin >> Ip;

	if ("Q" == Ip
		|| "q" == Ip)
	{
		Ip = "127.0.0.1";
	}

	SOCKADDR_IN Add = { 0, };
	Add.sin_family = AF_INET;
	Add.sin_port = htons(30001);

	if (SOCKET_ERROR == inet_pton(AF_INET, Ip.c_str(), &Add.sin_addr))
		return 0;

	if (SOCKET_ERROR == connect(SessionSocket, reinterpret_cast<const sockaddr*>(&Add), sizeof(SOCKADDR_IN)))
	{
		std::cout << "Ŀ��Ʈ �����߽��ϴ�." << std::endl;
		return 0;
	}

	std::cout << "Ŀ��Ʈ ����." << std::endl;

	_getch();

	return 0;
}