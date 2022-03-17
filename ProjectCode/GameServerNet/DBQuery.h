#pragma once
#include "DBConnecter.h"

class DBQuery
{
protected: // Member Var
	DBConnecter* m_DBConnecter;
	const char* m_QueryString;

public: // Default
	DBQuery(const char* query);
	virtual ~DBQuery() = default;

	DBQuery(const DBQuery& other) = delete;
	DBQuery(DBQuery&& other) noexcept = delete;

	DBQuery& operator=(const DBQuery& other) = delete;
	DBQuery& operator=(DBQuery&& other) = delete;
	
public:
	// m_QueryString을 DB에 보내는 함수
	virtual bool ExecuteQuery() = 0;
};

