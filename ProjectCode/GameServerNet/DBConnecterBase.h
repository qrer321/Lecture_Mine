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

public: // Member Function
	MYSQL* m_mysql;	
};

