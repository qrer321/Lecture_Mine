#pragma once
#include "Enums.h"

// 용도 : 
// 분류 :
// 첨언 : 주소체계를 담당하는 클래스라면 가져야 할 기본 인터페이스
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

