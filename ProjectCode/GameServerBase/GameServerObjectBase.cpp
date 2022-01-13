#include "PreCompile.h"
#include "GameServerObjectBase.h"

GameServerObjectBase::GameServerObjectBase()
	: m_Parent(nullptr)
{
    // vector 자료구조는 reserve()를 사용하지 않고 사용하면
    // 굉장히 비효율적일 때가 많다.
    m_LinkObject.reserve(5);
}

GameServerObjectBase::~GameServerObjectBase()
= default;

GameServerObjectBase::GameServerObjectBase(GameServerObjectBase&& other) noexcept
    : m_Parent(other.m_Parent)
{

}

bool GameServerObjectBase::IsLowLevelValid()
{
    if (nullptr == this)
    {
        return false;
    }

    return true;
}

