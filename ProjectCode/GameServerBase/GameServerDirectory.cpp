#include "PreCompile.h"
#include "GameServerDirectory.h"
#include "GameServerFile.h"
#include "GameServerDebug.h"
#include "GameServerString.h"

GameServerDirectory::GameServerDirectory()
{
	m_Path = std::filesystem::current_path();
}

std::string GameServerDirectory::GetDirectoryName() const
{
	return m_Path.filename().string();
}

std::vector<GameServerFile> GameServerDirectory::GetAllFile(const std::string& filter) const
{
	std::string second_filter;
	if (std::string::npos == filter.find('.'))
	{
		second_filter = ".";
	}

	second_filter += filter;
	GameServerString::ToUpper(second_filter);

	std::vector<GameServerFile> vector_file;
	const std::filesystem::directory_iterator directory_iterator = std::filesystem::directory_iterator(m_Path);
	for (const std::filesystem::directory_entry& file : directory_iterator)
	{
		std::string extension = file.path().extension().string();
		GameServerString::ToUpper(extension);

		if ("*" != filter && extension != second_filter)
		{
			continue;
		}

		vector_file.emplace_back(GameServerFile(file.path()));
	}

	return vector_file;
}

bool GameServerDirectory::IsRootDirectory() const
{
	return m_Path == m_Path.root_directory();
}

void GameServerDirectory::MoveToRootDirectory()
{
	m_Path = m_Path.root_directory();
}

void GameServerDirectory::MoveToParent()
{
	m_Path = m_Path.parent_path();
}

bool GameServerDirectory::MoveToParent(const std::string& dir_name)
{
	while (false == IsRootDirectory())
	{
		if (m_Path.filename().string() == dir_name)
		{
			return true;
		}

		MoveToParent();
	}

	GameServerDebug::AssertDebugMsg("존재하지 않는 경로로 이동했습니다.");
	return false;
}

bool GameServerDirectory::MoveToChild(const std::string& dir_name)
{
	m_Path.append(dir_name);

	if (false == IsExist())
	{
		GameServerDebug::AssertDebugMsg("존재하지 않는 경로로 이동했습니다.");
		return false;
	}

	return true;
}


std::string GameServerDirectory::AddFileNameToPath(const std::string& file_name)
{
	std::filesystem::path new_path = m_Path;
	new_path.append(file_name);

	return new_path.string();
}
