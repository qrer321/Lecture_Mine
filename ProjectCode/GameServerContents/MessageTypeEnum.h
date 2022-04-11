#pragma once

enum class MessageType
{
	Chat,
	PlayerUpdate,
	LoginResult,
	JoinResult,
	CreateCharacterResult,
	SelectCharacterResult,
	InsertSectionResult,
	CharacterList,
	ServerDestroy,
	ObjectDestroy,
	PlayersUpdate,
	EnemyUpdate,
	Login,
	Join,
	CreateCharacter,
	SelectCharacter,
	ClientToReady,
	MAX
};