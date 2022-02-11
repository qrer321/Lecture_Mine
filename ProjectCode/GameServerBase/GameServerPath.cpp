#include "PreCompile.h"
#include "GameServerPath.h"

GameServerPath::GameServerPath()
	: m_Path("")
{
}

GameServerPath::GameServerPath(std::filesystem::path path)
	: m_Path(std::move(path))
{
}

bool GameServerPath::IsExist() const
{
	return std::filesystem::exists(m_Path);
}

std::string GameServerPath::GetFileName(const std::string& path)
{
	std::filesystem::path new_path = path;
	return new_path.filename().string();
}

std::string GameServerPath::GetFileName() const
{
	return m_Path.filename().string();
}

std::string GameServerPath::GetFileNameWithoutExtension() const
{
	std::string extension = m_Path.extension().string();
	std::string file_name = m_Path.filename().string();
	return file_name.erase(file_name.size() - extension.size(), extension.size());
}

std::string GameServerPath::GetFullPath() const
{
	return m_Path.string();
}