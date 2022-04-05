#pragma once
#include <GameServerBase/GameServerObjectBase.h>
#include "UserTable.h"
#include "ContentsStructure.h"

class ContentsUserData : public GameServerObjectBase
{
public: // Member Var
	UserRow						m_UserRow;
	std::vector<FCharacterInfo> m_CharactersInfo{};

public: // Default
	ContentsUserData() = default;
	~ContentsUserData() override = default;

	ContentsUserData(const ContentsUserData& other) = delete;
	ContentsUserData(ContentsUserData&& other) noexcept = delete;
	ContentsUserData& operator=(const ContentsUserData& other) = delete;
	ContentsUserData& operator=(ContentsUserData&& other) = delete;
};

