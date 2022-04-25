#include "PreCompile.h"
#include "Portal.h"

#include <GameServerCore/GameServerSection.h>
#include <GameServerCore/GameServerCollision.h>
#include "ContentsSystemEnum.h"

Portal::Portal(FVector4 pos, FVector4 scale)
{
	SetActorPos(pos);
	SetActorScale(scale);
}

void Portal::SectionInitialize()
{
	const auto& collision = GetSection()->CreateCollision(ECollisionGroup::PORTAL, this);
	collision->SetScale({ 320.f, 320.f, 320.f });
}

void Portal::TCPSessionInitialize()
{
}

void Portal::UDPSessionInitialize()
{
}

void Portal::Update(float delta_time)
{
}
