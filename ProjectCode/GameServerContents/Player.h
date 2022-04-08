#pragma once
#include <GameServerCore/GameServerActor.h>
#include "ServerAndClient.h"

class ContentsUserData;
class Player : public GameServerActor
{
private: // Member Var
	PlayerUpdateMessage					m_UpdateMessage;
	GameServerSerializer				m_Serializer;

	std::shared_ptr<ContentsUserData>	m_UserData;

public: // Default
	Player() = default;
	~Player() override = default;

	Player(const Player& other) = delete;
	Player(Player&& other) noexcept = delete;
	Player& operator=(const Player& other) = delete;
	Player& operator=(Player&& other) = delete;

protected:
	PlayerUpdateMessage& GetPlayerUpdateMessage();
	GameServerSerializer& GetPlayerUpdateSerializer();

	void PlayerUpdateBroadcasting();

private:
	void SessionInitialize() override;
	void SectionInitialize() override;

	void Update(float delta_time) override;

	bool InsertSection() override;

public: // Member Function
};

