#pragma once

class ServerHelper
{
public:
	static void ServerStartup();

public: // Default
	ServerHelper() = default;
	~ServerHelper() = default;

	ServerHelper(const ServerHelper& other) = delete;
	ServerHelper(ServerHelper&& other) noexcept = delete;

	ServerHelper& operator=(const ServerHelper& other) = delete;
	ServerHelper& operator=(ServerHelper&& other) = delete;
};