#pragma once

class DBConnecter
{
private: // Member Var
	MYSQL*       m_mysql{};
	std::string  m_Host;
	std::string  m_ID;
	std::string  m_PW;
	std::string  m_Schema;
	unsigned int m_Port{};

public: // Default
	DBConnecter() = default;
	~DBConnecter() = default;

	DBConnecter(const DBConnecter& other) = delete;
	DBConnecter(DBConnecter&& other) noexcept = delete;

	DBConnecter& operator=(const DBConnecter& other) = delete;
	DBConnecter& operator=(DBConnecter&& other) = delete;

private:
	void SetConnectInfo(
		const std::string& host,
		const std::string& id,
		const std::string& pw,
		const std::string& schema,
		unsigned int port);

	bool Connect();

public: // Member Function
	bool Connect(
		const std::string& host,
		const std::string& id,
		const std::string& pw,
		const std::string& schema,
		unsigned int port);

	void Query(const std::string& query);

	[[nodiscard]] bool IsConnected() const { return m_mysql != nullptr; }
	[[nodiscard]] std::string GetLastSQLError() const;
};

