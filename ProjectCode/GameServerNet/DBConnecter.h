#pragma once

class DBConnecter;
class DBStatement : public std::enable_shared_from_this<DBStatement>
{
private:
	DBConnecter*					m_DBConnecter;

	MYSQL_STMT*						m_Statement;
	std::string_view				m_Query;

	// 특정할 수 없는 정보들은 MYSQL_BIND 구조체를 사용하여 bind할 수 있다.
	std::vector<MYSQL_BIND>			m_ParamBinds;
	std::vector<MYSQL_BIND>			m_ResultBinds;

	// Query의 ? Placeholder에 bind 하기 위한 정보체를 만들어준다.
	std::vector<unsigned long>		m_ParamLengthBuffer;
	std::vector<char>				m_ParamIsNullBuffer;
	std::vector<char>				m_ParamBindBuffer;

	std::vector<unsigned long>		m_ResultLengthBuffer;
	std::vector<char>				m_ResultIsNullBuffer;
	std::vector<char>				m_ResultBindBuffer;

public:
	DBStatement(DBConnecter* connecter, MYSQL_STMT* statement, const std::string_view& query);
	~DBStatement() = default;

	DBStatement(const DBStatement& other) = delete;
	DBStatement(DBStatement&& other) noexcept = delete;

	DBStatement& operator=(const DBStatement& other) = delete;
	DBStatement& operator=(DBStatement&& other) = delete;

public:
	void ParamBind_String(const std::string_view& value);
	void ParamBind_Int(const int value);
	void ParamBind_Float(const float value);

	void Execute();
};

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
	[[nodiscard]] bool IsConnected() const { return m_mysql != nullptr; }
	[[nodiscard]] std::string GetLastSQLError() const;

	bool Connect(
		const std::string& host,
		const std::string& id,
		const std::string& pw,
		const std::string& schema,
		unsigned int port);

	std::unique_ptr<DBStatement> CreateStatement(const std::string_view& query);
};

