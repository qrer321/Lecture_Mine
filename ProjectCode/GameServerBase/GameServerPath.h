#pragma once
#include <filesystem>

class GameServerPath
{
protected:
	std::filesystem::path m_Path;

public: // Default
	GameServerPath();
	explicit GameServerPath(std::filesystem::path path);
	~GameServerPath() = default;

	GameServerPath(const GameServerPath& other) = default;
	GameServerPath(GameServerPath&& other) noexcept = default;

	GameServerPath& operator=(const GameServerPath& other) = delete;
	GameServerPath& operator=(GameServerPath&& other) = delete;

public:
	[[nodiscard]] bool IsExist() const;

	static std::string GetFileName(const std::string& path);
	[[nodiscard]] std::string GetFileName() const;

	[[nodiscard]] std::string GetFileNameWithoutExtension() const;
	[[nodiscard]] std::string GetFullPath() const;

	
};

