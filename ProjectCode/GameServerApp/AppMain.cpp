#include "PreCompile.h"
#include <iostream>
#include <conio.h>
#include <GameServerBase/GameServerDebug.h>
#include <GameServerBase/GameServerIocp.h>
#include <GameServerBase/GameServerQueue.h>
#include <GameServerNet/TCPListener.h>
#include <GameServerNet/TCPSession.h>
#include <GameServerNet/ServerHelper.h>

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	ServerHelper::StartEngineStartUp();
	GameServerDebug::Initialize();

	// �����ڸ� �ޱ� ���� �񵿱� ���� �ʱ�ȭ
	PtrSTCPListener listener = std::make_shared<TCPListener>();
	listener->Initialize(IPEndPoint(IPAddress::Parse("127.0.0.1"), 30001), [](PtrSTCPSession s)
		{
			// �������� ó���� ������
			// � ������ ������ ������
			// �� ������ ó���Ѵ�
			// (���ø����̼� ����)

			// ������ ������ ��������, ä�η� ������ �������� ����

			// �����ڰ� �����ϰ� �ش� �����ڿ� ������ �����ϸ�
			// ���������� ó���� �Լ��� �� ��

			// ������ ����� �����ڴ� IP / PW�� �����ϰ�
			// DB�� ���� ������ ������ �޾ƾ� �Ѵ�
			s->SetCallBack([](PtrSTCPSession callback, const std::vector<char>& value)
				{
					std::string strTest = &value[0];
					GameServerDebug::LogInfo(strTest);
				}, nullptr);
			GameServerDebug::LogInfo("�����ڰ� �ֽ��ϴ�");
		});

	GameServerQueue networkQueue;
	networkQueue.Initialize(GameServerQueue::WORK_TYPE::Default, 1, "Network");
	listener->BindQueue(networkQueue);

	// ������ �ޱ� ����
	listener->StartAccept(1);

	_getch();

	networkQueue.Destroy();
	GameServerDebug::Destroy();
	
	return 0;
}