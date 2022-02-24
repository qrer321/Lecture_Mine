#pragma once
#include <vector>
#include <string>
#include "GameServerMathStruct.h"

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
	void operator<<(const FVector& value);

	void operator>>(std::string& value);
	void operator>>(int& value);
	void operator>>(unsigned int& value);
	void operator>>(FVector& value);

public:
	const std::vector<unsigned char>& GetData() { return m_Data; }

public:
	template <typename T>
	void ReadEnum(T& value) { Read(reinterpret_cast<void*>(&value), static_cast<unsigned int>(sizeof(T))); }
	void Read(void* data, unsigned int size);

	template <typename T>
	void WriteEnum(const T value) { Write(reinterpret_cast<const void*>(&value), static_cast<unsigned int>(sizeof(T))); }
	void Write(const void* data, unsigned int size);
};