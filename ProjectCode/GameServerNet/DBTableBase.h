#pragma once
#include "DBConnecter.h"

class DBTableBase
{
private:
	DBConnecter* m_Connecter;

public: // Default
	DBTableBase(DBConnecter* connecter);
	~DBTableBase() = default;

	DBTableBase(const DBTableBase& other) = delete;
	DBTableBase(DBTableBase&& other) noexcept = delete;

	DBTableBase& operator=(const DBTableBase& other) = delete;
	DBTableBase& operator=(DBTableBase&& other) = delete;
};