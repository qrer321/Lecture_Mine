#pragma once

enum class MessageType
{
	Chat,
	PlayerUpdate,
	LoginResult,
	JoinResult,
	CreateCharacterResult,
	SelectCharacterResult,
	UDPReadyOK,
	InsertSectionResult,
	CharacterList,
	ServerDestroy,
	PlayerDestroy,
	ObjectDestroy,
	PlayersUpdate,
	EnemyUpdate,
	Login,
	Join,
	CreateCharacter,
	SelectCharacter,
	ClientToReady,
	UDPConnectResult,
	MAX
};