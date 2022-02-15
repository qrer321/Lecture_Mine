#pragma once

enum class MessageType
{
	// C2S_Start,
	Login,
	Quit,
	// C2S_End,

	// SC2SC_Start,
	Chat,
	Attack,
	// SC2SC_End,

	// S2C_Start,
	LoginResult,
	ServerDestroy,
	// S2C_End,

	MAX,
};