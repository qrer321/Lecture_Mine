#pragma once

// 용도 : 
// 분류 :
// 첨언 : 
class GameServerCore
{
protected:
	static int			s_ServerPort;
	static int			s_DBPort;
	static std::string	s_DBHost;
	static std::string	s_DBSchema;
	static std::string	s_DBUser;
	static std::string	s_DBPw;

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

private:
	static bool CoreInit();
	static bool CoreEnd();

	static bool CoreDataCheck();

public: // Member Function
	template<typename UserGameType, typename... Types>
	static void CoreStart(Types... args)
	{
		CoreInit();

		UserGameType new_user_server = UserGameType(args...);
		new_user_server.UserStart();

		CoreEnd();
	}
};

