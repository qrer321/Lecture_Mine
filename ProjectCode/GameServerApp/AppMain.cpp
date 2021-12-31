#include "PreCompile.h"
#include <iostream>
#include <GameServerBase/GameServerDebug.h>
#include "TestClass.h"

int main()
{
	TestClass* NewClass = nullptr;

	if (!NewClass->IsValid())
	{
		GameServerDebug::AssertDebugMsg("터지는거 확인");
	}

	return 0;
}