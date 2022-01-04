#include "PreCompile.h"
#include "GameServerNameBase.h"

GameServerNameBase::GameServerNameBase() = default;

GameServerNameBase::~GameServerNameBase() = default;

GameServerNameBase::GameServerNameBase(GameServerNameBase&& other) noexcept
{

}

void GameServerNameBase::SetName(const std::string& name)
{
	m_Name = name;
}

std::string_view GameServerNameBase::GetNameView() const
{
	// string_view�� string�� ���� ���簡 �Ͼ�� �ʴ´�.
	return m_Name;
}

std::string GameServerNameBase::GetNameCopy() const
{
	return m_Name;
}

const char* GameServerNameBase::GetNamePtr() const
{
	return m_Name.c_str();
}

