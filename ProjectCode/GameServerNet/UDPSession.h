#pragma once
#include <GameServerBase/GameServerObjectBase.h>
#include <GameServerBase/GameServerObjectPool.h>
#include "IPEndPoint.h"

class UDPRecvOverlapped;
class UDPSendOverlapped;
class UDPSession : public GameServerObjectBase
{
	friend UDPRecvOverlapped;
	friend UDPSendOverlapped;

private: // Member Var
	SOCKET		m_SessionSocket{};
	SOCKADDR_IN m_RemoteAddr{};
	int			m_AddrSize{};

	GameServerObjectPool<UDPSendOverlapped> m_SendOverlappedPool;
	UDPRecvOverlapped*						m_RecvOverlapped{};
	
	std::function<void(std::shared_ptr<UDPSession> , const std::vector<unsigned char>&, IPEndPoint&)> m_RecvCallBack;

public: // Default
	UDPSession() = default;
	~UDPSession() override;

	UDPSession(const UDPSession& other) = delete;
	UDPSession(UDPSession&& other) noexcept = delete;
	UDPSession& operator=(const UDPSession& other) = delete;
	UDPSession& operator=(UDPSession&& other) = delete;

private:
	static void OnCallBack(BOOL result, DWORD number_of_bytes, LPOVERLAPPED overlapped);

	void OnSendComplete(UDPSendOverlapped* send_overlapped);
	void OnRecv(const char* recv_buffer, DWORD number_of_bytes);

public: // Member Function
	bool Send(const std::vector<unsigned char>& send_data, const IPEndPoint& remote_end_point);
	bool Recv();

	bool BindQueue(const GameServerQueue& task_queue);
};

