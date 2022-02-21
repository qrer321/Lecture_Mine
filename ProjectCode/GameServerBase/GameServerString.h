#pragma once

// 용도 : 
// 분류 :
// 첨언 : 
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
	static std::string UTF8ToAnsi(const std::string& utf8);
	static bool AnsiToUTF8(const std::string& ansi, std::string& utf8);

	static std::vector<std::string> Split(const std::string& input, char delimiter);
	static void Replace(std::string& source, const std::string& find_string, const std::string& replace_string, int replace_count = 0);
	static void Insert(std::string& source, int index, const std::string& insert_text);
	static void TrimRemove(std::string& source);
	static void ToUpper(std::string& source);
	static void Remove(std::string& source, const std::string& delete_text);
	static void TextClear(std::string& source);
};

