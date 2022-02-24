#pragma once

#include <d3d11.h>
#include <DirectXCollision.h>

struct FVector
{
	union
	{
		struct
		{
			float X;
			float Y;
			float Z;
		};

		float Arr[3];
	};
};