#pragma once
#include "Enums.h"

// �뵵 : 
// �з� :
// ÷�� : �ּ�ü�踦 ����ϴ� Ŭ������� ������ �� �⺻ �������̽�
class SocketAddress;
class EndPoint
{
public: // Default
	EndPoint() = default;
	virtual ~EndPoint() = 0 {}

public:
	[[nodiscard]] virtual AddressFamily GetAddressFamily() const = 0;
	[[nodiscard]] virtual SocketAddress Serialize() const = 0;
	[[nodiscard]] virtual uint64_t GetHashCode() const = 0;
};

