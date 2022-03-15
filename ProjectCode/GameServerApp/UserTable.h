#pragma once

class UserRow : std::enable_shared_from_this<UserRow>
{
public:
	int			m_Index;
	std::string m_ID;
	std::string m_PW;
};

class UserTable
{
private: // Member Var

public: // Default
	UserTable() = default;
	~UserTable() = default;

	UserTable(const UserTable& other) = delete;
	UserTable(UserTable&& other) noexcept = delete;

	UserTable& operator=(const UserTable& other) = delete;
	UserTable& operator=(UserTable&& other) = delete;

private:
	std::shared_ptr<UserRow> GetUserData(const std::string& id);

public: // Member Function
};

