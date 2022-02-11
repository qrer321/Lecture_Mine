#include "PreCompile.h"
#include "GameServerString.h"
#include "GameServerDebug.h"

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

void GameServerString::ToUpper(std::string& string)
{
	std::transform(string.begin(), string.end(), string.begin(), 
		[](unsigned char c)
		{
			return static_cast<unsigned char>(std::toupper(c));
		});
}
