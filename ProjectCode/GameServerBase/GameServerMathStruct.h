#pragma once

#include <d3d11.h>
#include <DirectXCollision.h>

struct FVector
{
public:
	static const FVector ZeroVector;
	///** A zero vector (0,0,0) */
	//static CORE_API const FVector ZeroVector;

	///** One vector (1,1,1) */
	//static CORE_API const FVector OneVector;

	///** Unreal up vector (0,0,1) */
	//static CORE_API const FVector UpVector;

	///** Unreal down vector (0,0,-1) */
	//static CORE_API const FVector DownVector;

	///** Unreal forward vector (1,0,0) */
	//static CORE_API const FVector ForwardVector;

	///** Unreal backward vector (-1,0,0) */
	//static CORE_API const FVector BackwardVector;

	///** Unreal right vector (0,1,0) */
	//static CORE_API const FVector RightVector;

	///** Unreal left vector (0,-1,0) */
	//static CORE_API const FVector LeftVector;

	///** Unit X axis vector (1,0,0) */
	//static CORE_API const FVector XAxisVector;

	///** Unit Y axis vector (0,1,0) */
	//static CORE_API const FVector YAxisVector;

	///** Unit Z axis vector (0,0,1) */
	//static CORE_API const FVector ZAxisVector;

public:
	union
	{
		struct
		{
			float X;
			float Y;
			float Z;
		};

		float Arr[3]{};
	};

	FVector& operator+=(const FVector& value)
	{
		X += value.X;
		Y += value.Y;
		Z += value.Z;

		return *this;
	}

	FVector()
		: X(0.f)
		, Y(0.f)
		, Z(0.f)
	{
	}

	FVector(float x, float y)
		: X(x)
		, Y(y)
		, Z(0.f)
	{
	}


	FVector(float x, float y, float z)
		: X(x)
		, Y(y)
		, Z(z)
	{
	}
};