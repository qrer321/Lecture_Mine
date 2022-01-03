#include "PreCompile.h"
#include "GameServerObjectBase.h"

GameServerObjectBase::GameServerObjectBase()
	: m_Parent(nullptr)
{
    // vector �ڷᱸ���� reserve()�� ������� �ʰ� ����ϸ�
    // ������ ��ȿ������ ���� ����.
    m_LinkObject.reserve(5);
}

GameServerObjectBase::~GameServerObjectBase() 
{

}

GameServerObjectBase::GameServerObjectBase(GameServerObjectBase&& _Other) noexcept
    : m_Parent(_Other.m_Parent)
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

