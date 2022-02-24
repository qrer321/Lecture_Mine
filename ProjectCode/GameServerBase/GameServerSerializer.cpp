#include "PreCompile.h"
#include "GameServerSerializer.h"

GameServerSerializer::GameServerSerializer()
	: m_Offset(0)
{
	m_Data.resize(1024);
}

GameServerSerializer::GameServerSerializer(const std::vector<unsigned char>& data)
	: m_Offset(0)
	, m_Data(data)
{
}

GameServerSerializer::GameServerSerializer(GameServerSerializer&& other) noexcept
	: m_Offset(other.m_Offset)
	, m_Data(std::move(other.m_Data))
{
}

/*
 * @param
 * void*	data : GameServerSerializer�� m_Data ������ ���� �ּ�
 * unsigned int	 : data�� ũ��
 */
void GameServerSerializer::Read(void* data, unsigned int size)
{
	memcpy_s(data, size, &m_Data[m_Offset], size);
	m_Offset += size;
}

/*
 * @param
 * const void*	data : GameServerSerializer�� m_Data�� �Է��� ���� �ּ�
 * unsigned		int	 : data�� ũ��
 */
void GameServerSerializer::Write(const void* data, unsigned int size)
{
	memcpy_s(&m_Data[m_Offset], size, data, size);
	m_Offset += size;
}

/*
 * value�� size ������ m_Data�� �ְ�
 * value�� ������ �ִ� ������ m_Data�� �ִ� operator
 */
void GameServerSerializer::operator<<(const std::string& value)
{
	operator<<(static_cast<int>(value.size()));

	Write(&value[0], static_cast<unsigned int>(value.size()));
}

void GameServerSerializer::operator<<(const int value)
{
	Write(&value, sizeof(int));
}

void GameServerSerializer::operator<<(const unsigned int value)
{
	Write(&value, sizeof(unsigned int));
}

void GameServerSerializer::operator<<(const FVector& value)
{
	Write(&value, sizeof(FVector));
}

/*
 * GameServerSerializer ��ü�� m_Data ������
 * int(4) ����Ʈ��ŭ �о�� value�� ���� �ִ´�
 */
void GameServerSerializer::operator>>(std::string& value)
{
	int size;
	operator>>(size);
	value.resize(size);
	Read(&value[0], size);
}

void GameServerSerializer::operator>>(int& value)
{
	Read(&value, sizeof(int));
}

void GameServerSerializer::operator>>(unsigned int& value)
{
	Read(&value, sizeof(unsigned int));
}

void GameServerSerializer::operator>>(FVector& value)
{
	Read(&value, sizeof(FVector));
}
