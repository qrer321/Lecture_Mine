#include "PreCompile.h"
#include "GameServerOverlapped.h"
#include "TCPSession.h"

////////////////// GameServerOverlapped //////////////////
GameServerOverlapped::GameServerOverlapped()
	: m_Overlapped()
{
	ResetOverlapped();
}

LPWSAOVERLAPPED GameServerOverlapped::GetOverlapped()
{
	return &m_Overlapped;
}

void GameServerOverlapped::ResetOverlapped()
{
	memset(&m_Overlapped, 0x00, sizeof(WSAOVERLAPPED));
}

////////////////// AcceptExOverlapped //////////////////
AcceptExOverlapped::AcceptExOverlapped(const PtrSTCPSession& tcpSession)
	: m_Buffer{}
	, m_TCPSession(tcpSession)
{
}

AcceptExOverlapped::~AcceptExOverlapped()
= default;

void AcceptExOverlapped::Execute(BOOL result, DWORD byteSize)
{
	if (nullptr == m_TCPSession)
		GameServerDebug::LogError("������ ���õǾ����� ���� �������� ��ü�Դϴ�");

	// AcceptEx�� �ּҰ��� ���� ���ƿ��µ�
	// �ش� �ּҰ��� �̾Ƴ���
	SOCKADDR_IN* localAddress = nullptr;
	SOCKADDR_IN* remoteAddress = nullptr;

	int localLen = 0, remoteLen = 0;

	/*
	 * GetAcceptExSockaddrs(lpOutputBuffer, dwReceiveDataLength, dwLocalAddressLength, dwRemoteAddressLength
	 * , LocalSockaddr, LocalSockaddrLength, RemoteSockaddr, RemoteSockaddrLength)
	 *
	 * GetAcceptExSockaddrs�� AcceptEx �Լ��� ¦�� �̷�� �Լ��� AcceptEx�� ȣ���Ͽ�
	 * ���ϵ� ����(lpOutputBuffer)���� ���ð� ����Ʈ ��巹���� ������ ����� �Ѵ�.
	 * �� ���ۿ��� Ŭ���̾�Ʈ�� �����ϸ鼭 ���۵� �����Ϳ� ���ῡ ����� ���ð� ����Ʈ ��巹���� ����ִ�.
	 * �ּҴ� ���ۿ��� �������� ���ʿ� �پ��ִ�.
	 * GetAcceptExSockaddrs�� ȣ���ϸ� ���ۿ��� ������ SOCKADDR ����ü �������� �ּҸ� ��ȯ�� �� �ִ�.

	 *
	 * @param
	 * In PVOID			lpOutputBuffer			: AcceptEx �Լ��� ȣ��� ���� ���ῡ�� ���۵� ù ��° ������ ����� �����ϴ� ���ۿ� ���� ������
	 *											  AcceptEx �Լ��� ���޵� lpOutputBuffer �Ű������� �����ؾ� �Ѵ�
	 *
	 * In DWORD			dwReceiveDataLength		: ù ��° �����͸� �����ϴµ� ���Ǵ� ������ ����Ʈ ��
	 *											  AcceptEx �Լ��� ���޵� dwReceiveDataLength �Ű������� �����ؾ� �Ѵ�
	 *
	 * In DWORD			dwLocalAddressLength	: ���� �ּ� ������ ����� ����Ʈ ��
	 *											  AcceptEx �Լ��� ���޵� dwLocalAddressLength �Ű������� �����ؾ� �Ѵ�
	 *
	 * In DWORD			dwRemoteAddressLength	: ���� �ּ� ������ ����� ����Ʈ ��
	 *											  AcceptEx �Լ��� ���޵� dwRemoteAddressLength �Ű������� �����ؾ� �Ѵ�
	 *
	 * Out sockaddr**	LocalSockaddr			: ������ ���� �ּҸ� �����ϴ� sockaddr ����ü�� ���� ������
	 *											  �ش� �Ű������� �ݵ�� �����Ǿ�� �Ѵ�
	 *
	 * Out LPINT(int*)	LocalSockaddrLength		: ���� �ּ��� ũ��(����Ʈ)
	 *											  �ش� �Ű������� �ݵ�� �����Ǿ�� �Ѵ�
	 *
	 * Out sockaddr**	RemoteSockaddr			: ������ ���� �ּҸ� �����ϴ� sockaddr ������ ���� ������
	 *											  �ش� �Ű������� �ݵ�� �����Ǿ�� �Ѵ�
	 *
	 * Out LPINT(int*)	RemoteSockaddrLength	: ���� �ּ��� ũ��(����Ʈ)
	 *											  �ش� �Ű������� �ݵ�� �����Ǿ�� �Ѵ�
	 */
	GetAcceptExSockaddrs(GetBuffer()
		, 0
		, sizeof(sockaddr_in) + 16
		, sizeof(sockaddr_in) + 16
		, reinterpret_cast<sockaddr**>(&localAddress)
		, &localLen
		, reinterpret_cast<sockaddr**>(&remoteAddress)
		, &remoteLen);

	// localAddress�� remoteAddress�� ����Ʈ ����� ���´�
	char arrLocal[4];
	char arrRemote[4];

	// IPv4�� �������� �� char ���
	memcpy_s(arrLocal, 4, &localAddress->sin_addr.s_addr, 4);
	memcpy_s(arrRemote, 4, &remoteAddress->sin_addr.s_addr, 4);

	m_TCPSession->m_LocalAddress = IPEndPoint(localAddress->sin_addr.s_addr, htons(localAddress->sin_port));
	m_TCPSession->m_RemoteAddress = IPEndPoint(remoteAddress->sin_addr.s_addr, htons(remoteAddress->sin_port));
}