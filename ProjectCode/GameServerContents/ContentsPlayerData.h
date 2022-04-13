#pragma once
#include <GameServerBase/GameServerObjectBase.h>
#include "ContentsStructure.h"
#include "UserTable.h"

// ContentsPlayerData Ŭ���� �������� ������� ���
// �÷��� �ߴ� ������� ����
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