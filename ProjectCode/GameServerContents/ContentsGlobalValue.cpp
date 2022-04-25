#include "PreCompile.h"
#include "ContentsGlobalValue.h"

std::mutex				ContentsGlobalValue::s_PlayableMutex;
std::unordered_set<int> ContentsGlobalValue::s_PlayableCharacterIndex;

bool ContentsGlobalValue::RegistPlayable(int character_index)
{
	std::lock_guard lock(s_PlayableMutex);

	if (s_PlayableCharacterIndex.end() != s_PlayableCharacterIndex.find(character_index))
	{
		return false;
	}

	s_PlayableCharacterIndex.insert(character_index);
	return true;
}

bool ContentsGlobalValue::UnRegistPlayable(int character_index)
{
	std::lock_guard lock(s_PlayableMutex);

	if (s_PlayableCharacterIndex.end() == s_PlayableCharacterIndex.find(character_index))
	{
		return false;
	}

	s_PlayableCharacterIndex.erase(character_index);
	return true;
}
