#pragma once

class DBConnecter;
class DBStatement;
class DBStatementResult : public std::enable_shared_from_this<DBStatementResult>
{
	friend DBStatement;

private:
	DBConnecter*					m_DBConnecter;
	MYSQL_STMT*						m_Statement;
	std::string_view				m_Query;

	std::vector<MYSQL_BIND>			m_ResultBinds;

	std::vector<unsigned long>		m_ResultLengthBuffer;
	std::vector<char>				m_ResultIsNullBuffer;
	std::vector<char>				m_ResultBindBuffer;

public:
	DBStatementResult(DBConnecter* connecter, MYSQL_STMT* statement, const std::string_view& query);
	~DBStatementResult() = default;

	DBStatementResult(const DBStatementResult& other) = delete;
	DBStatementResult(DBStatementResult&& other) noexcept = delete;

	DBStatementResult& operator=(const DBStatementResult& other) = delete;
	DBStatementResult& operator=(DBStatementResult&& other) = delete;

public:
	uint64_t GetAffectedRow() const { return mysql_stmt_affected_rows(m_Statement); }
	std::string GetString(const int index) const;
	int GetInt(const int index) const;

	bool Next() const;
};

class DBStatement : public std::enable_shared_from_this<DBStatement>
{
private:
	DBConnecter*					m_DBConnecter;
	MYSQL_STMT*						m_Statement;
	std::string_view				m_Query;

	// Ư���� �� ���� �������� MYSQL_BIND ����ü�� ����Ͽ� bind�� �� �ִ�.
	std::vector<MYSQL_BIND>			m_ParamBinds;
	
	// Query�� ? Placeholder�� bind �ϱ� ���� ����ü�� ������ش�.
	std::vector<unsigned long>		m_ParamLengthBuffer;
	std::vector<char>				m_ParamIsNullBuffer;
	std::vector<char>				m_ParamBindBuffer;

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

	std::unique_ptr<DBStatementResult> Execute();

	// mysql_stmt_prepare���� ����� INSERT �Ǵ� UPDATE ���� ���� AUTO_INCREMENT ���� ������ ���� ��ȯ�Ѵ�.
	// AUTO_INCREMENT �ʵ尡 ���Ե� ���̺��� ����� INSERT ���� ������ �Ŀ� ����ϴ� �Լ�
	[[nodiscard]] uint64_t GetInsertID() const { return mysql_stmt_insert_id(m_Statement); }

	// mysql_stmt_execute �Լ��� ���� ��ɹ� ���� ���Ŀ� ȣ�� �� �� �ִ�.
	// UPDATE, DELETE, INSERT ���� ��� ����, ���� �Ǵ� ���Ե� ���� ���� ��ȯ�Ѵ�.
	// ������ ��� -1�� ��ȯ�Ѵ�.
	[[nodiscard]] uint64_t GetAffectedRow() const { return mysql_stmt_affected_rows(m_Statement); }
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

	void Reset();
	bool Connect(
		const std::string& host,
		const std::string& id,
		const std::string& pw,
		const std::string& schema,
		unsigned int port);

	std::unique_ptr<DBStatement> CreateStatement(const std::string_view& query);
};

