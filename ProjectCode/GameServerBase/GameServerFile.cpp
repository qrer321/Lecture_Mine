#include "PreCompile.h"
#include "GameServerFile.h"
#include "GameServerDebug.h"

GameServerFile::GameServerFile()
	: m_FileHandle(nullptr)
{
}

GameServerFile::GameServerFile(const std::filesystem::path& path)
	: GameServerPath(path)
	, m_FileHandle(nullptr)
{
}

GameServerFile::GameServerFile(const std::string& path)
	: m_FileHandle(nullptr)
{
	m_Path = path;

	if (false == IsExist())
	{
		GameServerDebug::AssertDebug();
	}
}

GameServerFile::GameServerFile(const std::string& path, const std::string& mode)
	: m_FileHandle(nullptr)
{
	m_Path = path;

	Open(mode);

	if (false == IsExist())
	{
		GameServerDebug::AssertDebug();
	}
}

GameServerFile::~GameServerFile()
{
	Close();
}

GameServerFile::GameServerFile(GameServerFile&& other) noexcept
	: GameServerPath(std::move(other))
	, m_FileHandle(nullptr)
{
}

uintmax_t GameServerFile::GetFileSize() const
{
	return std::filesystem::file_size(m_Path);
}

std::string GameServerFile::FileName() const
{
	return m_Path.filename().string();
}

std::string GameServerFile::GetString() const
{
	std::string all_string = std::string();
	all_string.resize(GetFileSize());

	Read(&all_string[0], all_string.size(), all_string.size());

	while(0 == all_string[all_string.size() - 1])
	{
		all_string.erase(--all_string.end());
	}

	return all_string;
}

void GameServerFile::Open(const std::string& mode)
{
	m_OpenMode = mode;
	fopen_s(&m_FileHandle, m_Path.string().c_str(), m_OpenMode.c_str());
	if (nullptr == m_FileHandle)
	{
		GameServerDebug::AssertDebug();
	}
}

void GameServerFile::Close()
{
	if (nullptr != m_FileHandle)
	{
		fclose(m_FileHandle);
		m_FileHandle = nullptr;
	}
}

void GameServerFile::Write(const void* data, size_t size)
{
	if (m_OpenMode[0] != 'w')
	{
		GameServerDebug::AssertDebug();
		return;
	}

	if (nullptr == m_FileHandle)
	{
		GameServerDebug::AssertDebug();
		return;
	}

	fwrite(data, size, 1, m_FileHandle);
}

void GameServerFile::Write(const std::string& data)
{
	const int size = static_cast<int>(data.size());
	Write(&size, sizeof(int));
	Write(data.c_str(), data.size());
}

void GameServerFile::Write(const std::wstring& data)
{
	const int size = static_cast<int>(data.size() * 2);
	Write(&size, sizeof(int));
	Write(data.c_str(), data.size());
}

void GameServerFile::Write(const int& data)
{
	Write(&data, sizeof(int));
}

void GameServerFile::Read(void* buffer, size_t buffer_size, size_t data_size) const
{
	if (m_OpenMode[0] != 'r')
	{
		GameServerDebug::AssertDebug();
		return;
	}

	if (nullptr == m_FileHandle)
	{
		GameServerDebug::AssertDebug();
		return;
	}

	fread_s(buffer, buffer_size, data_size, 1, m_FileHandle);
}

void GameServerFile::Read(std::string& data)
{
	int size = 0;
	Read(&size, sizeof(int), sizeof(int));
	data.resize(size);
	Read(&data[0], size, size);
}

void GameServerFile::Read(int& data)
{
	Read(&data, sizeof(int), sizeof(int));
}
