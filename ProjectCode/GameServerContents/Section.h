#pragma once

// 유저 혹은 몬스터들을 하나의 방(지역, 섹터)으로 묶어 연산하기 위해 사용될 클래스
// Section 클래스는 해당 지역에 있을 유저나 몬스터들이 어떤 행동을 할지 알 수 없기에
// GameServerActor를 컨테이너로 관리한다.
class GameServerActor;
class Section
{
public:

public: // Default
	Section() = default;
	~Section() = default;

	Section(const Section& other) = delete;
	Section(Section&& other) noexcept = delete;

	Section& operator=(const Section& other) = delete;
	Section& operator=(Section&& other) = delete;

public: // Member Function
};

