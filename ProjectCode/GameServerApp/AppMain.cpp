#include "PreCompile.h"
#include <iostream>
#include <GameServerBase/GameServerDebug.h>
#include "TestClass.h"

int main()
{
	TestClass* NewClass = nullptr;

	if (!NewClass->IsValid())
	{
		GameServerDebug::AssertDebugMsg("�����°� Ȯ��");
	}

	return 0;
}