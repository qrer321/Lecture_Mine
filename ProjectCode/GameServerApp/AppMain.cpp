#include "PreCompile.h"
#include <GameServerCore/GameServerCore.h>
#include <GameServerContents/GameServerContentsCore.h>

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(25108);

	GameServerCore::CoreStart<GameServerContentsCore>();

	return 0;
}