#pragma once
#include <GameServerNet/TCPListener.h>

class TCPSession;
class GameServerCore
{
protected:
	static int												s_ServerPort;
	static int												s_DBPort;
	static std::string										s_DBHost;
	static std::string										s_DBSchema;
	static std::string										s_DBUser;
	static std::string										s_DBPw;

	static TCPListener										s_Listener;
	static std::function<void(std::shared_ptr<TCPSession>)> s_AcceptCallback;

public: // Default
	GameServerCore() = default;
	~GameServerCore() = default;

	GameServerCore(const GameServerCore& other) = delete;
	GameServerCore(GameServerCore&& other) noexcept = delete;
	GameServerCore& operator=(const GameServerCore& other) = delete;
	GameServerCore& operator=(GameServerCore&& other) = delete;

	static int			GetServerPort() { return s_ServerPort; }
	static int			GetDBPort()		{ return s_DBPort; }
	static std::string	GetDBHost()		{ return s_DBHost; }
	static std::string	GetDBSchema()	{ return s_DBSchema; }
	static std::string	GetDBUser()		{ return s_DBUser; }
	static std::string	GetDBPw()		{ return s_DBPw; }

protected:
	void SetAcceptCallback(const std::function<void(std::shared_ptr<TCPSession>)>& callback);

private:
	static bool CoreInit();
	static bool CoreRun();
	static bool CoreEnd();

	static bool CoreDataCheck();

public: // Member Function
	template<typename UserGameType, typename... Types>
	static void CoreStart(Types... args)
	{
		CoreInit();

		UserGameType new_user_server = UserGameType(args...);
		new_user_server.UserStart();

		CoreRun();

		CoreEnd();
	}
};

