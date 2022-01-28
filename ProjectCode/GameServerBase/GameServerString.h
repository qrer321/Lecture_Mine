#pragma once

// �뵵 : 
// �з� :
// ÷�� : 
class GameServerString
{
public: // Default
	GameServerString() = default;
	~GameServerString() = default;

	GameServerString(const GameServerString& other) = delete;
	GameServerString(GameServerString&& other) noexcept = default;

public:
	GameServerString& operator=(const GameServerString& other) = delete;
	GameServerString& operator=(GameServerString&& other) = delete;

public:
	static bool UTF8ToUnicode(const std::string& utf8, std::wstring& unicode);
	static bool UnicodeToAnsi(const std::wstring& unicode, std::string& ansi);

	static bool AnsiToUnicode(const std::string& ansi, std::wstring& unicode);
	static bool UnicodeToUTF8(const std::wstring& unicode, std::string& utf8);

	static bool UTF8ToAnsi(const std::string& utf8, std::string& ansi);
	static bool AnsiToUTF8(const std::string& ansi, std::string& utf8);
};

