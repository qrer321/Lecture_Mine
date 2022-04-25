#pragma once

#include <d3d11.h>
#include <DirectXCollision.h>

struct FVector4
{
public:
	static const FVector4 ZeroVector;
	///** A zero vector (0,0,0) */
	//static CORE_API const FVector4 ZeroVector;

	///** One vector (1,1,1) */
	//static CORE_API const FVector4 OneVector;

	///** Unreal up vector (0,0,1) */
	//static CORE_API const FVector4 UpVector;

	///** Unreal down vector (0,0,-1) */
	//static CORE_API const FVector4 DownVector;

	///** Unreal forward vector (1,0,0) */
	//static CORE_API const FVector4 ForwardVector;

	///** Unreal backward vector (-1,0,0) */
	//static CORE_API const FVector4 BackwardVector;

	///** Unreal right vector (0,1,0) */
	//static CORE_API const FVector4 RightVector;

	///** Unreal left vector (0,-1,0) */
	//static CORE_API const FVector4 LeftVector;

	///** Unit X axis vector (1,0,0) */
	//static CORE_API const FVector4 XAxisVector;

	///** Unit Y axis vector (0,1,0) */
	//static CORE_API const FVector4 YAxisVector;

	///** Unit Z axis vector (0,0,1) */
	//static CORE_API const FVector4 ZAxisVector;

public:
	union
	{
		struct
		{
			float X;
			float Y;
			float Z;
			float W;
		};

		float Arr[3];

		DirectX::XMVECTOR dxVector{};
	};

	FVector4& operator+=(const FVector4& value)
	{
		X += value.X;
		Y += value.Y;
		Z += value.Z;
		W += value.W;

		return *this;
	}

	FVector4 operator-(const FVector4& value)
	{
		return { X - value.X, Y - value.Y, Z - value.Z };
	}

	FVector4 operator+(const FVector4& value)
	{
		return { X + value.X, Y + value.Y, Z + value.Z };
	}

	static float Length2D(const FVector4& value)
	{
		const DirectX::XMVECTOR vec = DirectX::XMVector2Length(value.dxVector);
		return vec.m128_f32[0];
	}

	float Length2D() const
	{
		const DirectX::XMVECTOR vec = DirectX::XMVector2Length(dxVector);
		return vec.m128_f32[0];
	}

	FVector4 Sub2D(const FVector4& value) const
	{
		return { X - value.X, Y - value.Y };
	}

	FVector4 Sub3D(const FVector4& value)
	{
		return operator-(value);
	}

	std::string ToString() const
	{
		return "X : " + std::to_string(X) + " Y : " + std::to_string(Y) + " Z : " + std::to_string(Z);
	}

	FVector4 HalfVector3D()
	{
		return { X * 0.5f, Y * 0.5f, Z * 0.5f };
	}

	DirectX::XMFLOAT3 ConvertXMFloat3()
	{
		return { X, Y, Z };
	}

	DirectX::XMFLOAT4 ConvertXMFloat4()
	{
		return { X, Y, Z, W };
	}

	FVector4()
		: X(0.f)
		, Y(0.f)
		, Z(0.f)
		, W(0.f)
	{
	}

	FVector4(float x, float y)
		: X(x)
		, Y(y)
		, Z(0.f)
		, W(0.f)
	{
	}


	FVector4(float x, float y, float z)
		: X(x)
		, Y(y)
		, Z(z)
		, W(0.f)
	{
	}

	FVector4(float x, float y, float z, float w)
		: X(x)
		, Y(y)
		, Z(z)
		, W(w)
	{
	}
};