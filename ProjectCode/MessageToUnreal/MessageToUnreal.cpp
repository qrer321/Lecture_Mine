// MessageToUnreal.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <iostream>
#include <GameServerBase/GameServerFile.h>
#include <GameServerBase/GameServerString.h>
#include <GameServerBase/GameServerDirectory.h>

#pragma comment (lib, "GameServerBase.lib")

struct MessageInfo
{
	std::string					m_Name;
	std::vector<std::string>	m_Member;
};

void MessageReflection(std::vector<MessageInfo>& collection, const std::string& code)
{
	const std::vector<std::string> message_vector = GameServerString::Split(code, '|');
	for (const auto& message_string : message_vector)
	{
		if (true == message_string.empty())
		{
			continue;
		}

		std::vector<std::string> name_and_member = GameServerString::Split(message_string, '-');
		const std::string& name = name_and_member[0];
		const std::string& member_string = name_and_member[1];

		MessageInfo message_info;
		message_info.m_Name = name;

		std::vector<std::string> members = GameServerString::Split(member_string, ';');
		for (auto& member : members)
		{
			GameServerString::TextClear(member);
			if (true == member.empty())
			{
				continue;
			}

			message_info.m_Member.push_back(member);
		}

		collection.push_back(message_info);
	}
}

int main()
{
	{
		GameServerDirectory file_dir;
		file_dir.MoveToParent("ProjectCode");
		file_dir.MoveToChild(R"(GameServerMessage\MessageInfo)");

		std::vector<MessageInfo> all_message;
		std::vector<MessageInfo> server_client_message;
		std::vector<MessageInfo> server_message;
		std::vector<MessageInfo> client_message;

		{
			GameServerFile load_file = { file_dir.AddFileNameToPath("MessageServerAndClient.txt"), "rt" };
			std::string file_code = load_file.GetString();
			MessageReflection(server_client_message, file_code);
		}

		{
			GameServerFile load_file = { file_dir.AddFileNameToPath("MessageServer.txt"), "rt" };
			std::string file_code = load_file.GetString();
			MessageReflection(server_message, file_code);
		}

		{
			GameServerFile load_file = { file_dir.AddFileNameToPath("MessageClient.txt"), "rt" };
			std::string file_code = load_file.GetString();
			MessageReflection(client_message, file_code);
		}

		all_message.reserve(server_client_message.size() + server_message.size() + client_message.size());
		for (const auto& message_element : server_client_message)
		{
			all_message.emplace_back(message_element);
		}

		for (const auto& message_element : server_message)
		{
			all_message.emplace_back(message_element);
		}

		for (const auto& message_element : client_message)
		{
			all_message.emplace_back(message_element);
		}


		/////////////////////////////////////// Create Enum File ///////////////////////////////////////
		{
			GameServerDirectory enum_file_dir;
			enum_file_dir.MoveToParent("ProjectCode");
			enum_file_dir.MoveToChild("GameServerMessage");

			std::string enum_file_text = "#pragma once\n\nenum class MessageType\n{\n";

			for (const auto& message_element : all_message)
			{
				enum_file_text += "\t" + message_element.m_Name + ",\n";
			}

			enum_file_text += "};";

			GameServerFile save_file = { enum_file_dir.AddFileNameToPath("MessageTypeEnum.h"), "wt" };
			save_file.Write(enum_file_text.c_str(), enum_file_text.size());
		}
		////////////////////////////////////////////////////////////////////////////////////////////////

		/////////////////////////////////////// Create Convert File ///////////////////////////////////////
		{
			GameServerDirectory convert_file_dir;
			convert_file_dir.MoveToParent("ProjectCode");
			convert_file_dir.MoveToChild("GameServerMessage");

			std::string convert_file_text = "#include \"PreCompile.h\"\n";
			convert_file_text += "#include \"MessageConverter.h\"\n\n";
			convert_file_text += "MessageConverter::MessageConverter(const std::vector<unsigned char>& buffer)\n";
			convert_file_text += "\t: m_Buffer(buffer)\n";
			convert_file_text += "{\n";
			convert_file_text += "\tGameServerSerializer serializer = GameServerSerializer(buffer);\n";
			convert_file_text += "\tMessageType type;\n";
			convert_file_text += "\n";
			convert_file_text += "\tmemcpy_s(&type, sizeof(MessageType), &buffer[0], sizeof(MessageType));\n";
			convert_file_text += "\tswitch (type)\n";
			convert_file_text += "\t{\n";

			for (const auto& message_element : all_message)
			{
				convert_file_text += "\tcase MessageType::" + message_element.m_Name + ":\n";
				convert_file_text += "\t\tm_Message = std::make_shared<" + message_element.m_Name + "Message>();\n";
				convert_file_text += "\t\tbreak;\n";
			}

			convert_file_text += "\tdefault:\n";
			convert_file_text += "\t\treturn; \n\t}\n\n";
			convert_file_text += "\tif (nullptr == m_Message)\n\t{\n\t\treturn;\n\t}\n\n";
			convert_file_text += "\tm_Message->Deserialize(serializer);\n}";

			GameServerFile save_file = { convert_file_dir.AddFileNameToPath("MessageConverter.cpp"), "wt" };
			save_file.Write(convert_file_text.c_str(), convert_file_text.size());
		}
		///////////////////////////////////////////////////////////////////////////////////////////////////
	}


	return 0;







	// 단순복사
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