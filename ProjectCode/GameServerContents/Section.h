#pragma once

// ���� Ȥ�� ���͵��� �ϳ��� ��(����, ����)���� ���� �����ϱ� ���� ���� Ŭ����
// Section Ŭ������ �ش� ������ ���� ������ ���͵��� � �ൿ�� ���� �� �� ���⿡
// GameServerActor�� �����̳ʷ� �����Ѵ�.
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

