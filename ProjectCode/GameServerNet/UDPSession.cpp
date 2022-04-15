#include "PreCompile.h"
#include "UDPSession.h"
#include "GameServerOverlapped.h"
#include "SocketAddress.h"

UDPSession::~UDPSession()
{
	delete m_RecvOverlapped;
}

bool UDPSession::BindQueue(const GameServerQueue& task_queue)
{
	return task_queue.NetworkBind(m_SessionSocket,
		[&](auto&& result, auto&& number_of_bytes, auto&& overlapped)
		{
			OnCallBack(std::forward<decltype(result)>(result),
			           std::forward<decltype(number_of_bytes)>(number_of_bytes),
			           std::forward<decltype(overlapped)>(overlapped));
		});
}

void UDPSession::OnCallBack(BOOL result, DWORD number_of_bytes, LPOVERLAPPED overlapped)
{
	if (nullptr == overlapped)
	{
		return;
	}

	const auto server_overlapped = reinterpret_cast<GameServerOverlapped*>(reinterpret_cast<char*>(overlapped) - sizeof(void*));
	server_overlapped->Execute(result, number_of_bytes);
}

void UDPSession::OnSendComplete(UDPSendOverlapped* send_overlapped)
{
	m_SendOverlappedPool.Push(send_overlapped);
}

void UDPSession::OnRecv(const char* recv_buffer, DWORD number_of_bytes)
{
	if (nullptr == m_RecvCallBack)
	{
		return;
	}

	std::vector<unsigned char> buffer;
	buffer.resize(number_of_bytes);
	memcpy_s(&buffer[0], buffer.size(), recv_buffer, number_of_bytes);

	IPEndPoint remote_end_point(m_RemoteAddr.sin_addr.s_addr, htons(m_RemoteAddr.sin_port));
	m_RecvCallBack(DynamicCast<UDPSession>(), buffer, remote_end_point);

	Recv();
}

bool UDPSession::Send(const std::vector<unsigned char>& send_data, const IPEndPoint& remote_end_point)
{
	if (send_data.empty())
	{
		return false;
	}

	UDPSendOverlapped* send_overlapped = m_SendOverlappedPool.Pop();
	send_overlapped->SetUDPSession(DynamicCast<UDPSession>());
	send_overlapped->CopyFrom(send_data);

	DWORD sent_bytes = 0;

	SocketAddress socket_address = remote_end_point.Serialize();
	if (SOCKET_ERROR == WSASendTo(
		m_SessionSocket,
		send_overlapped->GetWSABuffer(),
		1,
		&sent_bytes,
		0,
		reinterpret_cast<sockaddr*>(&socket_address),
		static_cast<int>(socket_address.GetBufferLength()),
		send_overlapped->GetOverlapped(),
		nullptr
	))
	{
		if (WSA_IO_PENDING != WSAGetLastError())
		{
			GameServerDebug::GetLastErrorPrint();
			return false;
		}
	}

	return true;
}

bool UDPSession::Recv()
{
	if (INVALID_SOCKET == m_SessionSocket)
	{
		return false;
	}

	DWORD recvd_bytes = 0, flags = 0;

	if (SOCKET_ERROR == WSARecvFrom(
		m_SessionSocket,
		m_RecvOverlapped->GetWSABuffer(),
		1,
		&recvd_bytes,
		&flags,
		reinterpret_cast<sockaddr*>(&m_RemoteAddr),
		&m_AddrSize,
		m_RecvOverlapped->GetOverlapped(),
		nullptr))
	{
		if (WSA_IO_PENDING != WSAGetLastError())
		{
			GameServerDebug::GetLastErrorPrint();
			return false;
		}
	}

	return true;
}
