#include "PreCompile.h"
#include "GameServerString.h"
#include "GameServerDebug.h"
#include <sstream>

bool GameServerString::UTF8ToUnicode(const std::string& utf8, std::wstring& unicode)
{
	unicode.clear();

	int len = MultiByteToWideChar(
		CP_UTF8, 
		0,
		utf8.c_str(),
		static_cast<int>(utf8.size()),
		nullptr,
		0);

	if (0 >= len)
	{
		GameServerDebug::GetLastErrorPrint();
		return false;
	}

	unicode.resize(len);

	len = MultiByteToWideChar(
		CP_UTF8,
		0,
		utf8.c_str(),
		static_cast<int>(utf8.size()),
		&unicode[0],
		len);

	if (0 >= len)
	{
		GameServerDebug::GetLastErrorPrint();
		return false;
	}

	return true;
}

bool GameServerString::UnicodeToAnsi(const std::wstring& unicode, std::string& ansi)
{
	ansi.clear();

	int len = WideCharToMultiByte(
		CP_ACP,
		0,
		unicode.c_str(),
		static_cast<int>(unicode.size()),
		nullptr,
		0,
		nullptr,
		nullptr);

	if (0 >= len)
	{
		GameServerDebug::GetLastErrorPrint();
		return false;
	}

	ansi.resize(len);

	len = WideCharToMultiByte(
		CP_ACP,
		0,
		unicode.c_str(),
		static_cast<int>(unicode.size()),
		&ansi[0],
		len,
		nullptr,
		nullptr);

	if (0 >= len)
	{
		GameServerDebug::GetLastErrorPrint();
		return false;
	}

	return true;
}

bool GameServerString::AnsiToUnicode(const std::string& ansi, std::wstring& unicode)
{
	unicode.clear();

	int len = MultiByteToWideChar(
		CP_ACP,
		0,
		ansi.c_str(),
		static_cast<int>(ansi.size()),
		nullptr,
		0);

	if (0 >= len)
	{
		GameServerDebug::GetLastErrorPrint();
		return false;
	}

	unicode.resize(len);

	len = MultiByteToWideChar(
		CP_ACP,
		0,
		ansi.c_str(),
		static_cast<int>(ansi.size()),
		&unicode[0],
		len);

	if (0 >= len)
	{
		GameServerDebug::GetLastErrorPrint();
		return false;
	}

	return true;
}

bool GameServerString::UnicodeToUTF8(const std::wstring& unicode, std::string& utf8)
{
	utf8.clear();

	int len = WideCharToMultiByte(
		CP_UTF8,
		0,
		unicode.c_str(),
		static_cast<int>(unicode.size()),
		nullptr,
		0,
		nullptr,
		nullptr);

	if (0 >= len)
	{
		GameServerDebug::GetLastErrorPrint();
		return false;
	}

	utf8.resize(len);

	len = WideCharToMultiByte(
		CP_UTF8,
		0,
		unicode.c_str(),
		static_cast<int>(unicode.size()),
		&utf8[0],
		len,
		nullptr,
		nullptr);

	if (0 >= len)
	{
		GameServerDebug::GetLastErrorPrint();
		return false;
	}

	return true;
}

bool GameServerString::UTF8ToAnsi(const std::string& utf8, std::string& ansi)
{
	std::wstring unicode;
	if (false == UTF8ToUnicode(utf8, unicode))
	{
		GameServerDebug::AssertDebugMsg("UTF8 => Unicode Convert Error");
		return false;
	}
	if (false == UnicodeToAnsi(unicode, ansi))
	{
		GameServerDebug::AssertDebugMsg("Unicode => Ansi Convert Error");
		return false;
	}

	return true;
}

std::string GameServerString::UTF8ToAnsi(const std::string& utf8)
{
	std::string ansi;
	std::wstring unicode;
	if (false == UTF8ToUnicode(utf8, unicode))
	{
		GameServerDebug::AssertDebugMsg("UTF8 => Unicode Convert Error");
	}
	if (false == UnicodeToAnsi(unicode, ansi))
	{
		GameServerDebug::AssertDebugMsg("Unicode => Ansi Convert Error");
	}

	return ansi;
}

bool GameServerString::AnsiToUTF8(const std::string& ansi, std::string& utf8)
{
	std::wstring unicode;
	if (false == AnsiToUnicode(ansi, unicode))
	{
		GameServerDebug::AssertDebugMsg("Ansi => Unicode Convert Error");
		return false;
	}
	if (false == UnicodeToUTF8(unicode, utf8))
	{
		GameServerDebug::AssertDebugMsg("Unicode => UTF8 Convert Error");
		return false;
	}

	return true;
}

std::vector<std::string> GameServerString::Split(const std::string& input, char delimiter)
{
	std::vector<std::string> result;
	std::stringstream ss(input);
	std::string temp;

	while(getline(ss, temp, delimiter))
	{
		result.push_back(temp);
	}

	return result;
}

void GameServerString::Replace(std::string& source, const std::string& find_string, const std::string& replace_string, int replace_count /*= 0*/)
{
	std::string::size_type string_pos = 0;
	int count = 0;

	while ((string_pos = source.find(find_string)) != std::string::npos)
	{
		source.replace(string_pos, find_string.length(), replace_string);

		if (++count == replace_count)
		{
			break;
		}
	}
}

void GameServerString::Insert(std::string& source, int index, const std::string& insert_text)
{
	if (static_cast<int>(source.length()) < index || 0 > index)
	{
		return;
	}

	source.insert(index, insert_text);
}

// ' ' 공백제거
void GameServerString::TrimRemove(std::string& source)
{
	Replace(source, " ", "");
}

// 한글이 섞여있어도 알파벳 전부 upper
void GameServerString::ToUpper(std::string& source)
{
	std::transform(source.begin(), source.end(), source.begin(),
		[](char word)
		{
			/*
			 * __NTH (toupper (int __c))
			 * {
			 *	return __c >= -128 && __c < 256 ? (*__ctype_toupper_loc ())[__c] : __c;
			 * }
			 */
			return static_cast<char>(std::toupper(word));
		});
}

void GameServerString::Remove(std::string& source, const std::string& delete_text)
{
	Replace(source, delete_text, "");
}

void GameServerString::TextClear(std::string& source)
{
	Replace(source, "\t", "");
	Replace(source, "\n", "");
}