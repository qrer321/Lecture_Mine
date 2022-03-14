#pragma once

class DBConnecterBase
{
private: // Member Var

public: // Default
	DBConnecterBase() = default;
	~DBConnecterBase() = default;

	DBConnecterBase(const DBConnecterBase& other) = delete;
	DBConnecterBase(DBConnecterBase&& other) noexcept = delete;

	DBConnecterBase& operator=(const DBConnecterBase& other) = delete;
	DBConnecterBase& operator=(DBConnecterBase&& other) = delete;

private:
	MYSQL* m_mysql;

public: // Member Function
	void Connect(
		const std::string& host,
		const std::string& id,
		const std::string& pw,
		const std::string& schema,
		unsigned int port);
};

