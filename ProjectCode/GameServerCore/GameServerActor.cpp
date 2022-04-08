#include "PreCompile.h"
#include "GameServerActor.h"

GameServerActor::GameServerActor()
	: m_Section(nullptr)
	, m_ActorIndex(-1)
	, m_ThreadIndex(-1)
	, m_SectionIndex(-1)
	, m_ActorPos(FVector::ZeroVector)
	, m_ActorDir(FVector::ZeroVector)
{
}
