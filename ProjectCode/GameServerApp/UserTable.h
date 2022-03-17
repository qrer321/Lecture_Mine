#pragma once
#include <GameServerNet/DBQuery.h>

class UserRow : std::enable_shared_from_this<UserRow>
{
public:
	int			m_Index;
	std::string m_ID;
	std::string m_PW;
};

class UserTable
{
protected:
	static std::string	s_TableName;
	std::string			m_Query;

public: // Default
	UserTable() = default;
	~UserTable() = default;

	UserTable(const UserTable& other) = delete;
	UserTable(UserTable&& other) noexcept = delete;

	UserTable& operator=(const UserTable& other) = delete;
	UserTable& operator=(UserTable&& other) = delete;

private:
	std::shared_ptr<UserRow> GetUserData(const std::string& id);
};

class UserTable_SelectIDFromUserInfo : public DBQuery, UserTable
{
private:
	std::string m_ID;

public: // Default
	UserTable_SelectIDFromUserInfo(std::string id);
	~UserTable_SelectIDFromUserInfo() override = default;

	UserTable_SelectIDFromUserInfo(const UserTable_SelectIDFromUserInfo& other) = delete;
	UserTable_SelectIDFromUserInfo(UserTable_SelectIDFromUserInfo&& other) noexcept = delete;

	UserTable_SelectIDFromUserInfo& operator=(const UserTable_SelectIDFromUserInfo& other) = delete;
	UserTable_SelectIDFromUserInfo& operator=(UserTable_SelectIDFromUserInfo&& other) = delete;

public:
	bool ExecuteQuery() override;
};