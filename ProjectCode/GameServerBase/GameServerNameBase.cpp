#include "PreCompile.h"
#include "GameServerNameBase.h"

GameServerNameBase::GameServerNameBase() 
{

}

GameServerNameBase::~GameServerNameBase() 
{

}

GameServerNameBase::GameServerNameBase(GameServerNameBase&& _Other) noexcept
{

}

void GameServerNameBase::SetName(const std::string& name)
{
	m_Name = name;
}

std::string_view GameServerNameBase::GetNameView()
{
	// string_view�� string�� ���� ���簡 �Ͼ�� �ʴ´�.
	return m_Name;
}

std::string GameServerNameBase::GetNameCopy()
{
	return m_Name;
}

const char* GameServerNameBase::GetNamePtr()
{
	return m_Name.c_str();
}

