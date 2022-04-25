#include "PreCompile.h"
#include "GameServerObjectBase.h"

GameServerObjectBase::GameServerObjectBase()
	: m_Parent(nullptr)
	, m_IsUpdate(true)
	, m_IsDeath(false)
	, m_AccTime(0.f)
{
    // vector �ڷᱸ���� reserve()�� ������� �ʰ� ����ϸ�
    // ������ ��ȿ������ ���� ����.
    m_LinkObject.reserve(5);
}

GameServerObjectBase::~GameServerObjectBase()
= default;

GameServerObjectBase::GameServerObjectBase(GameServerObjectBase&& other) noexcept
    : m_Parent(other.m_Parent)
    , m_LinkObject(std::move(other.m_LinkObject))
    , m_AccTime(other.m_AccTime)
	, m_Frame(0)
{
    other.m_Parent = nullptr;
    other.m_LinkObject.clear();
    const bool temp_update = m_IsUpdate;
    const bool temp_death = m_IsDeath;
    other.m_IsUpdate = temp_update;
    other.m_IsDeath = temp_death;
}

bool GameServerObjectBase::IsValidLowLevel()
{
    if (nullptr == this)
    {
        return false;
    }

    return true;
}

