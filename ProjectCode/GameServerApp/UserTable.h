#pragma once
#include <GameServerNet/DBQuery.h>

class UserRow : std::enable_shared_from_this<UserRow>
{
public:
	int			m_Index{};
	std::string m_ID;
	std::string m_PW;

public:
	UserRow() = default;
	UserRow(const int index, std::string id, std::string pw)
		: m_Index(index), m_ID(std::move(id)), m_PW(std::move(pw))
	{
	}

	UserRow(const UserRow& other) = delete;
	UserRow(UserRow&& other) noexcept = delete;

	UserRow& operator=(const UserRow& other) = delete;
	UserRow& operator=(UserRow&& other) = delete;
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
public:
	std::string					m_ID;
	std::shared_ptr<UserRow>	m_RowDatum;

public: // Default
	explicit UserTable_SelectIDFromUserInfo(std::string id);
	~UserTable_SelectIDFromUserInfo() override = default;

	UserTable_SelectIDFromUserInfo(const UserTable_SelectIDFromUserInfo& other) = delete;
	UserTable_SelectIDFromUserInfo(UserTable_SelectIDFromUserInfo&& other) noexcept = delete;

	UserTable_SelectIDFromUserInfo& operator=(const UserTable_SelectIDFromUserInfo& other) = delete;
	UserTable_SelectIDFromUserInfo& operator=(UserTable_SelectIDFromUserInfo&& other) = delete;

public:
	bool ExecuteQuery() override;
};

class UserTable_AllUserInfo : public DBQuery, UserTable
{
public:
	std::vector<std::shared_ptr<UserRow>>	m_RowData;

public: // Default
	explicit UserTable_AllUserInfo();
	~UserTable_AllUserInfo() override = default;

	UserTable_AllUserInfo(const UserTable_AllUserInfo& other) = delete;
	UserTable_AllUserInfo(UserTable_AllUserInfo&& other) noexcept = delete;

	UserTable_AllUserInfo& operator=(const UserTable_AllUserInfo& other) = delete;
	UserTable_AllUserInfo& operator=(UserTable_AllUserInfo&& other) = delete;

public:
	bool ExecuteQuery() override;
};

class UserTable_InsertToUserInfo : public DBQuery, UserTable
{
private:
	std::string m_ID;
	std::string m_PW;

public: // Default
	UserTable_InsertToUserInfo(std::string id, std::string pw);
	~UserTable_InsertToUserInfo() override = default;

	UserTable_InsertToUserInfo(const UserTable_InsertToUserInfo& other) = delete;
	UserTable_InsertToUserInfo(UserTable_InsertToUserInfo&& other) noexcept = delete;

	UserTable_InsertToUserInfo& operator=(const UserTable_InsertToUserInfo& other) = delete;
	UserTable_InsertToUserInfo& operator=(UserTable_InsertToUserInfo&& other) = delete;

public:
	bool ExecuteQuery() override;
};