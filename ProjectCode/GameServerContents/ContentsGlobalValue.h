#pragma once

class ContentsGlobalValue
{
private: // Member Var
	static std::mutex				s_PlayableMutex;
	static std::unordered_set<int>	s_PlayableCharacterIndex;

public: // Default
	ContentsGlobalValue() = default;
	~ContentsGlobalValue() = default;

	ContentsGlobalValue(const ContentsGlobalValue& other) = delete;
	ContentsGlobalValue(ContentsGlobalValue&& other) noexcept = delete;
	ContentsGlobalValue& operator=(const ContentsGlobalValue& other) = delete;
	ContentsGlobalValue& operator=(ContentsGlobalValue&& other) = delete;

public: // Member Function
	static bool RegistPlayable(int character_index);
	static bool UnRegistPlayable(int character_index);
};

