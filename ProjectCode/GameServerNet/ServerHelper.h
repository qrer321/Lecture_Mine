#pragma once

// �뵵 : 
// �з� :
// ÷�� : 
class ServerHelper
{
public:
	static void StartEngineStartUp();

public: // Default
	ServerHelper();
	~ServerHelper();

	ServerHelper(const ServerHelper& other) = delete;
	ServerHelper(ServerHelper&& other) noexcept;

public:
	ServerHelper& operator=(const ServerHelper& other) = delete;
	ServerHelper& operator=(ServerHelper&& other) = delete;

private:

public: // Member Function
};

