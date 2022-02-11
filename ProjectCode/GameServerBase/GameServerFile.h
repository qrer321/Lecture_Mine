#pragma once
#include "GameServerPath.h"
#include <vector>
#include <list>
#include <map>

class GameServerFile : public GameServerPath
{
private: // Member Var
	FILE*		m_FileHandle;
	std::string m_OpenMode;

public: // Default
	GameServerFile();
	GameServerFile(const std::filesystem::path& path);
	GameServerFile(const std::string& path);
	GameServerFile(const std::string& path, const std::string& mode);
	~GameServerFile();

	GameServerFile(const GameServerFile& other) = delete;
	GameServerFile(GameServerFile&& other) noexcept;

	GameServerFile& operator=(const GameServerFile& other) = delete;
	GameServerFile& operator=(GameServerFile&& other) = delete;

public:
	[[nodiscard]] uintmax_t GetFileSize() const;
	[[nodiscard]] std::string FileName() const;
	[[nodiscard]] std::string GetString() const;

public: // Member Function
	void Open(const std::string& mode);
	void Close();

	void Write(const void* data, size_t size);
	void Write(const std::string& data);
	void Write(const std::wstring& data);
	void Write(const int& data);
	void Write(const size_t& data) = delete;

	void Read(void* buffer, size_t buffer_size, size_t data_size) const;
	void Read(std::string& data);
	void Read(int& data);


	template <typename DataType>
	void Write(std::vector<DataType>& data);

	template <typename DataType>
	void Write(std::list<DataType>& data);

	template <typename KeyType, typename DataType>
	void Write(std::map<KeyType, DataType>& data);

	template <typename DataType>
	void Read(std::vector<DataType>& data);

	template <typename DataType>
	void Read(std::list<DataType>& data);

	template <typename KeyType, typename DataType>
	void Read(std::map<KeyType, DataType>& data);
};

template <typename DataType>
void GameServerFile::Write(std::vector<DataType>& data)
{
	const int size = static_cast<int>(data.size());
	Write(&size, sizeof(int));

	for (size_t i = 0; i < data.size(); ++i)
	{
		Write(data[i]);
	}
}

template <typename DataType>
void GameServerFile::Write(std::list<DataType>& data)
{
	const int size = static_cast<int>(data.size());
	Write(&size, sizeof(int));

	typename std::list<DataType>::iterator begin = data.begin();
	typename std::list<DataType>::iterator end = data.end();

	for (; begin != end; ++begin)
	{
		Write(*begin);
	}
}

template <typename KeyType, typename DataType>
void GameServerFile::Write(std::map<KeyType, DataType>& data)
{
	const int size = static_cast<int>(data.size());
	Write(&size, sizeof(int));

	typename std::map<KeyType, DataType>::iterator begin = data.begin();
	typename std::map<KeyType, DataType>::iterator end = data.end();

	for (; begin != end; ++begin)
	{
		Write(begin->frist);
		Write(begin->second);
	}
}

template <typename DataType>
void GameServerFile::Read(std::vector<DataType>& data)
{
	int size = 0;
	Read(&size, sizeof(int), sizeof(int));

	for (int i = 0; i < size; ++i)
	{
		DataType buffer;
		Read(buffer);
		data.push_back(buffer);
	}
}

template <typename DataType>
void GameServerFile::Read(std::list<DataType>& data)
{
	int size = 0;
	Read(&size, sizeof(int), sizeof(int));

	for (int i = 0; i < size; ++i)
	{
		DataType buffer;
		Read(buffer);
		data.push_back(buffer);
	}
}

template <typename KeyType, typename DataType>
void GameServerFile::Read(std::map<KeyType, DataType>& data)
{
	int size = 0;
	Read(&size, sizeof(int), sizeof(int));

	for (int i = 0; i < size; ++i)
	{
		KeyType key_buffer;
		Read(key_buffer);

		DataType data_buffer;
		Read(data_buffer);

		data.insert(std::map<KeyType, DataType>::value_type(key_buffer, data_buffer));
	}
}
