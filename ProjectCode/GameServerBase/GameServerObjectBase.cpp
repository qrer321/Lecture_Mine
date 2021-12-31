#include "PreCompile.h"
#include "GameServerObjectBase.h"

GameServerObjectBase::GameServerObjectBase() 
{

}

GameServerObjectBase::~GameServerObjectBase() 
{

}

GameServerObjectBase::GameServerObjectBase(GameServerObjectBase&& _Other) noexcept
{

}

bool GameServerObjectBase::IsValid()
{
    if (nullptr == this)
    {
        return false;
    }

    return true;
}

