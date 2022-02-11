#pragma once
#include <vector>
#include <string>

class GameServerSerializer
{
private: // Member Var
	unsigned int				m_Offset;
	std::vector<unsigned char>	m_Data;

public: // Default
	GameServerSerializer();
	explicit GameServerSerializer(const std::vector<unsigned char>& data);
	~GameServerSerializer() = default;

	GameServerSerializer(const GameServerSerializer& other) = delete;
	GameServerSerializer(GameServerSerializer&& other) noexcept;

	GameServerSerializer& operator=(const GameServerSerializer& other) = delete;
	GameServerSerializer& operator=(GameServerSerializer&& other) = delete;

public:
	void operator<<(const std::string& value);
	void operator<<(const int value);
	void operator<<(const unsigned int value);

	void operator>>(std::string& value);
	void operator>>(int& value);
	void operator>>(unsigned int& value);

public:
	const std::vector<unsigned char>& GetData() { return m_Data; }

public:
	void Read(void* data, unsigned int size);
	void Write(const void* data, unsigned int size);
};