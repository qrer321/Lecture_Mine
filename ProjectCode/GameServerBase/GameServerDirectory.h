#pragma once
#include "GameServerPath.h"

class GameServerFile;
class GameServerDirectory : public GameServerPath
{
public: // Default
	GameServerDirectory();
	~GameServerDirectory() = default;

	GameServerDirectory(const GameServerDirectory& other) = default;
	GameServerDirectory(GameServerDirectory&& other) noexcept = default;

	GameServerDirectory& operator=(const GameServerDirectory& other) = delete;
	GameServerDirectory& operator=(GameServerDirectory&& other) = delete;

public:
	[[nodiscard]] std::string GetDirectoryName() const;
	[[nodiscard]] std::vector<GameServerFile> GetAllFile(const std::string& filter) const;

	[[nodiscard]] bool IsRootDirectory() const;
	[[nodiscard]] bool IsExistFile(const std::string& file_name) const;

public:
	void MoveToRootDirectory();
	void MoveToParent();
	bool MoveToParent(const std::string& dir_name);
	bool MoveToChild(const std::string& dir_name);

	std::string AddFileNameToPath(const std::string& file_name) const;
};

