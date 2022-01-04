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
	// string_view는 string의 버퍼 복사가 일어나지 않는다.
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

