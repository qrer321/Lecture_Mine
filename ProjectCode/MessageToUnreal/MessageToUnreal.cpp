// MessageToUnreal.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <iostream>
#include <GameServerBase/GameServerFile.h>
#include <GameServerBase/GameServerString.h>
#include <GameServerBase/GameServerDirectory.h>

#pragma comment (lib, "GameServerBase.lib")

int main()
{
	{
		GameServerDirectory file_dir;
		file_dir.MoveToParent();
		file_dir.MoveToChild("GameServerBase");

		GameServerDirectory save_dir;
		save_dir.MoveToParent();
		save_dir.MoveToParent();
		save_dir.MoveToParent();
		save_dir.MoveToParent();
		save_dir.MoveToChild(R"(UnrealProject_Mine\HonorProject 4.27\Source\HonorProject\Message)");


		/////////////////////////////////////// GameServerSerializer ///////////////////////////////////////
		{
			GameServerFile load_file = { file_dir.AddFileNameToPath("GameServerSerializer.h"), "rt" };
			std::string file_code = load_file.GetString();

			GameServerFile save_file = { save_dir.AddFileNameToPath("GameServerSerializer.h"), "wt" };
			save_file.Write(file_code.c_str(), file_code.size());
		}

		{
			GameServerFile load_file = { file_dir.AddFileNameToPath("GameServerSerializer.cpp"), "rt" };
			std::string file_code = load_file.GetString();

			file_code.erase(0, strlen("#include \"PreCompile.h\"") + 1);

			GameServerFile save_file = { save_dir.AddFileNameToPath("GameServerSerializer.cpp"), "wt" };
			save_file.Write(file_code.c_str(), file_code.size());
		}
	}

	{
		GameServerDirectory file_dir;
		file_dir.MoveToParent();
		file_dir.MoveToChild("GameServerMessage");

		GameServerDirectory save_dir;
		save_dir.MoveToParent();
		save_dir.MoveToParent();
		save_dir.MoveToParent();
		save_dir.MoveToParent();
		save_dir.MoveToChild(R"(UnrealProject_Mine\HonorProject 4.27\Source\HonorProject\Message)");

		/////////////////////////////////////// Messages ///////////////////////////////////////
		{
			GameServerFile load_file = { file_dir.AddFileNameToPath("Messages.h"), "rt" };
			std::string file_code = load_file.GetString();

			file_code.replace(file_code.find("#include <GameServerBase/GameServerSerializer.h>\n")
				, strlen("#include <GameServerBase/GameServerSerializer.h>\n"), "#include \"GameServerSerializer.h\"\n");

			GameServerFile save_file = { save_dir.AddFileNameToPath("Messages.h"), "wt" };
			save_file.Write(file_code.c_str(), file_code.size());
		}

		{
			GameServerFile load_file = { file_dir.AddFileNameToPath("Messages.cpp"), "rt" };
			std::string file_code = load_file.GetString();

			file_code.erase(0, strlen("#include \"PreCompile.h\"") + 1);

			GameServerFile save_file = { save_dir.AddFileNameToPath("Messages.cpp"), "wt" };
			save_file.Write(file_code.c_str(), file_code.size());
		}

		/////////////////////////////////////// MessageTypeEnum ///////////////////////////////////////
		{
			GameServerFile load_file = { file_dir.AddFileNameToPath("MessageTypeEnum.h"), "rt" };
			std::string file_code = load_file.GetString();

			GameServerFile save_file = { save_dir.AddFileNameToPath("MessageTypeEnum.h"), "wt" };
			save_file.Write(file_code.c_str(), file_code.size());
		}

		/////////////////////////////////////// MessageConverter ///////////////////////////////////////
		{
			GameServerFile load_file = { file_dir.AddFileNameToPath("MessageConverter.h"), "rt" };
			std::string file_code = load_file.GetString();

			GameServerFile save_file = { save_dir.AddFileNameToPath("MessageConverter.h"), "wt" };
			save_file.Write(file_code.c_str(), file_code.size());
		}

		{
			GameServerFile load_file = { file_dir.AddFileNameToPath("MessageConverter.cpp"), "rt" };
			std::string file_code = load_file.GetString();

			file_code.erase(0, strlen("#include \"PreCompile.h\"") + 1);

			GameServerFile save_file = { save_dir.AddFileNameToPath("MessageConverter.cpp"), "wt" };
			save_file.Write(file_code.c_str(), file_code.size());
		}
	}

	{
		// MessageType Enum 분석
		GameServerDirectory file_dir;
		file_dir.MoveToParent();
		file_dir.MoveToChild("GameServerMessage");

		GameServerFile load_file = { file_dir.AddFileNameToPath("MessageTypeEnum.h"), "rt" };
		std::string file_code = load_file.GetString();

		std::vector<std::string> message_code = GameServerString::Split(file_code, ',');

		std::vector<std::string> C2S_string;
		std::vector<std::string> SC2SC_string;
		std::vector<std::string> S2C_string;
		std::map<std::string, std::vector<std::string>> message_map;

		message_map.insert(std::make_pair("C2S_Start", std::vector<std::string>()));
		message_map.insert(std::make_pair("SC2SC_Start", std::vector<std::string>()));
		message_map.insert(std::make_pair("S2C_Start", std::vector<std::string>()));

		std::vector<std::string>* find_pointer = nullptr;
		for (size_t i = 0; i < message_code.size(); ++i)
		{
			std::string type_check = message_code[i];
			if (std::string::npos != type_check.find("//"))
			{
				std::string text = type_check.substr(type_check.find("//") + strlen("//"), type_check.size());
				if (message_map.end() != message_map.find(text))
				{
					find_pointer = &message_map[text];

					++i;
					type_check = message_code[i];
				}
			}

			if (std::string::npos != type_check.find("End"))
			{
				find_pointer = nullptr;
			}

			if (nullptr != find_pointer)
			{
				find_pointer->push_back(type_check);
			}
		}

		/*{
			std::vector<std::string>& c2s = message_map["C2S_Start"];
			for (size_t i = 0; i < c2s.size(); ++i)
			{
				c2s[i].replace(c2s[i].begin(), c2s[i].end(), "\t", "");
			}
		}*/
	}
}