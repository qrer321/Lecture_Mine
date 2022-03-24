#pragma once

// ���� Ȥ�� ���͵��� �ϳ��� ��(����, ����)���� ���� �����ϱ� ���� ���� Ŭ����
// Room Ŭ������ �ش� ������ ���� ������ ���͵��� � �ൿ�� ���� �� �� ���⿡
// GameServerActor�� �����̳ʷ� �����Ѵ�.
class GameServerActor;
class Room
{
public:

public: // Default
	Room() = default;
	~Room() = default;

	Room(const Room& other) = delete;
	Room(Room&& other) noexcept = delete;

	Room& operator=(const Room& other) = delete;
	Room& operator=(Room&& other) = delete;

public: // Member Function
};

