#pragma once
#include <GameServerCore/GameServerCore.h>

class GameServerContentsCore : public GameServerCore
{
public: // Default
	GameServerContentsCore() = default;
	~GameServerContentsCore() = default;

	GameServerContentsCore(const GameServerContentsCore& other) = delete;
	GameServerContentsCore(GameServerContentsCore&& other) noexcept = delete;
	GameServerContentsCore& operator=(const GameServerContentsCore& other) = delete;
	GameServerContentsCore& operator=(GameServerContentsCore&& other) = delete;

public: // Member Function
	void UserStart();

protected:
	static void AcceptEvent(const std::shared_ptr<TCPSession>& session);
	static void RecvEvent(const std::shared_ptr<TCPSession>& session, const std::vector<unsigned char>& data);
	static void CloseEvent(const std::shared_ptr<TCPSession>& session);

	static void RecvEvent_UDP(const std::shared_ptr<UDPSession>& session, const std::vector<unsigned char>& data, const IPEndPoint& end_point);
};

