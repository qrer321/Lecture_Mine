#pragma once
#include <GameServerCore/GameServerActor.h>

class Portal : public GameServerActor
{
public:
	GameServerSection* m_LinkSection{};

public: // Default
	Portal(FVector4 pos, FVector4 scale);
	~Portal() override = default;

	Portal(const Portal& other) = delete;
	Portal(Portal&& other) noexcept = delete;

	Portal& operator=(const Portal& other) = delete;
	Portal& operator=(Portal&& other) = delete;

private:
	void SectionInitialize() override;
	void TCPSessionInitialize() override;
	void UDPSessionInitialize() override;

	void Update(float delta_time) override;
	bool InsertSection() override { return true; }
	void DeathEvent() override {}
};

