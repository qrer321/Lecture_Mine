#pragma once
#include <GameServerBase/GameServerObjectBase.h>
#include "ContentsStructure.h"
#include "UserTable.h"

// ContentsPlayerData 클래스 정보까지 들고있을 경우
// 플레이 했던 유저라고 판정
class Player;
class ContentsPlayerData : public GameServerObjectBase
{
public:
	Player* m_ConnectPlayer{};

public:
	ContentsPlayerData() = default;
	~ContentsPlayerData() override = default;

	ContentsPlayerData(const ContentsPlayerData& other) = delete;
	ContentsPlayerData(ContentsPlayerData&& other) noexcept = delete;
	ContentsPlayerData& operator=(const ContentsPlayerData& other) = delete;
	ContentsPlayerData& operator=(ContentsPlayerData&& other) noexcept = delete;
};