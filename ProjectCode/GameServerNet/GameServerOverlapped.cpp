#include "PreCompile.h"
#include "GameServerOverlapped.h"
#include "TCPSession.h"

////////////////// GameServerOverlapped //////////////////
GameServerOverlapped::GameServerOverlapped()
	: m_Overlapped()
{
	ResetOverlapped();
}

GameServerOverlapped::~GameServerOverlapped()
= default;

LPWSAOVERLAPPED GameServerOverlapped::GetOverlapped()
{
	return &m_Overlapped;
}

void GameServerOverlapped::ResetOverlapped()
{
	memset(&m_Overlapped, 0x00, sizeof(WSAOVERLAPPED));
}


////////////////// AcceptExOverlapped //////////////////
AcceptExOverlapped::AcceptExOverlapped(PtrSTCPSession tcpSession)
	: m_Buffer{}
	, m_TCPSession(std::move(tcpSession))
{
}

AcceptExOverlapped::~AcceptExOverlapped()
= default;

void AcceptExOverlapped::Execute(BOOL result, DWORD byteSize)
{
	if (nullptr == m_TCPSession)
		GameServerDebug::LogError("세션이 세팅되어있지 않은 오버랩드 객체입니다");

	// AcceptEx의 주소값이 같이 날아오는데
	// 해당 주소값을 뽑아낸다
	SOCKADDR_IN* localAddress = nullptr;
	SOCKADDR_IN* remoteAddress = nullptr;

	int localLen = 0, remoteLen = 0;

	/*
	 * GetAcceptExSockaddrs(lpOutputBuffer, dwReceiveDataLength, dwLocalAddressLength, dwRemoteAddressLength
	 * , LocalSockaddr, LocalSockaddrLength, RemoteSockaddr, RemoteSockaddrLength)
	 *
	 * GetAcceptExSockaddrs는 AcceptEx 함수와 짝을 이루는 함수로 AcceptEx를 호출하여
	 * 리턴된 버퍼(lpOutputBuffer)에서 로컬과 리모트 어드레스를 얻어오는 기능을 한다.
	 * 이 버퍼에는 클라이언트가 연결하면서 전송된 데이터와 연결에 사용한 로컬과 리모트 어드레스를 담고있다.
	 * 주소는 버퍼에서 데이터의 뒤쪽에 붙어있다.
	 * GetAcceptExSockaddrs를 호출하면 버퍼에서 적절한 SOCKADDR 구조체 형식으로 주소를 변환할 수 있다.

	 *
	 * @param
	 * In PVOID			lpOutputBuffer			: AcceptEx 함수의 호출로 인해 연결에서 전송된 첫 번째 데이터 블록을 수신하는 버퍼에 대한 포인터
	 *											  AcceptEx 함수에 전달된 lpOutputBuffer 매개변수와 동일해야 한다
	 *
	 * In DWORD			dwReceiveDataLength		: 첫 번째 데이터를 수신하는데 사용되는 버퍼의 바이트 수
	 *											  AcceptEx 함수에 전달된 dwReceiveDataLength 매개변수와 동일해야 한다
	 *
	 * In DWORD			dwLocalAddressLength	: 로컬 주소 정보에 예약된 바이트 수
	 *											  AcceptEx 함수에 전달된 dwLocalAddressLength 매개변수와 동일해야 한다
	 *
	 * In DWORD			dwRemoteAddressLength	: 원격 주소 정보에 예약된 바이트 수
	 *											  AcceptEx 함수에 전달된 dwRemoteAddressLength 매개변수와 동일해야 한다
	 *
	 * Out sockaddr**	LocalSockaddr			: 연결의 로컬 주소를 수신하는 sockaddr 구조체에 대한 포인터
	 *											  해당 매개변수는 반드시 지정되어야 한다
	 *
	 * Out LPINT(int*)	LocalSockaddrLength		: 로컬 주소의 크기(바이트)
	 *											  해당 매개변수는 반드시 지정되어야 한다
	 *
	 * Out sockaddr**	RemoteSockaddr			: 연결의 원격 주소를 수신하는 sockaddr 구조에 대한 포인터
	 *											  해당 매개변수는 반드시 지정되어야 한다
	 *
	 * Out LPINT(int*)	RemoteSockaddrLength	: 원격 주소의 크기(바이트)
	 *											  해당 매개변수는 반드시 지정되어야 한다
	 */
	GetAcceptExSockaddrs(GetBuffer()
		, 0
		, sizeof(sockaddr_in) + 16
		, sizeof(sockaddr_in) + 16
		, reinterpret_cast<sockaddr**>(&localAddress)
		, &localLen
		, reinterpret_cast<sockaddr**>(&remoteAddress)
		, &remoteLen);

	// localAddress와 remoteAddress는 바이트 덩어리로 나온다
	char arrLocal[4];
	char arrRemote[4];

	// IPv4로 형식으로 된 char 출력
	memcpy_s(arrLocal, 4, &localAddress->sin_addr.s_addr, 4);
	memcpy_s(arrRemote, 4, &remoteAddress->sin_addr.s_addr, 4);

	m_TCPSession->m_LocalAddress = IPEndPoint(localAddress->sin_addr.s_addr, htons(localAddress->sin_port));
	m_TCPSession->m_RemoteAddress = IPEndPoint(remoteAddress->sin_addr.s_addr, htons(remoteAddress->sin_port));
}


////////////////// RecvOverlapped //////////////////
RecvOverlapped::RecvOverlapped(PtrSTCPSession tcpSession)
	: m_Buffer{}
	, m_wsaBuffer()
	, m_TCPSession(std::move(tcpSession))
{
	Clear();
}

RecvOverlapped::~RecvOverlapped()
= default;

void RecvOverlapped::Clear()
{
	memset(m_Buffer, 0x00, sizeof(m_Buffer));

	m_wsaBuffer.len = sizeof(m_Buffer);
	m_wsaBuffer.buf = m_Buffer;
}

void RecvOverlapped::Execute(BOOL result, DWORD byteSize)
{
	// 넘어온 byteSize가 0일 경우는
	// 서버가 터졌다던가, 클라의 연결이 끊어졌다는 등
	// 잘못된 경우이기 때문에 무조건 정리한다
	if (0 == byteSize)
	{
		m_TCPSession->Close();
		return;
	}

	m_TCPSession->OnRecv(m_Buffer, byteSize);
}


////////////////// DisconnectOverlapped //////////////////
DisconnectOverlapped::DisconnectOverlapped(PtrSTCPSession tcpSession)
	: m_TCPSession(std::move(tcpSession))
{
}

DisconnectOverlapped::~DisconnectOverlapped()
= default;

void DisconnectOverlapped::Execute(BOOL result, DWORD byteSize)
{
	if (nullptr != m_TCPSession)
	{
		m_TCPSession->UnRegisterSession();
	}
}