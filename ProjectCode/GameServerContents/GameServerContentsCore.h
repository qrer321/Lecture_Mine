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
	static void RecvEvent(std::shared_ptr<TCPSession> session, const std::vector<unsigned char>& value);
	static void CloseEvent(std::shared_ptr<TCPSession> session);
};

