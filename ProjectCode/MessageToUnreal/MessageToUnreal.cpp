// MessageToUnreal.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <iostream>
#include <GameServerBase/GameServerDebug.h>
#include <GameServerBase/GameServerFile.h>
#include <GameServerBase/GameServerString.h>
#include <GameServerBase/GameServerDirectory.h>

#pragma comment (lib, "GameServerBase.lib")

// 폴더에 따라 변경해서 사용
#define UNREAL_CLIENT_DIR "UnrealClient\\Source\\HonorProject\\Message"

struct MemberInfo
{
	std::string MemberText;
	std::string Type;
	std::string Name;
};

struct MessageInfo
{
	std::string				MessageName;
	std::vector<MemberInfo>	MemberVariable;
};

void SerializerTypeCheck(std::string& source, const MemberInfo& member_info)
{
	if (member_info.Type == "std::string")
	{
		source += "		serializer << " + member_info.Name + ";\n";
	}
	else if (member_info.Type == "int")
	{
		source += "		serializer << " + member_info.Name + ";\n";
	}
	else
	{
		if (member_info.Type[0] == 'E')
		{
			source += "		serializer.WriteEnum(" + member_info.Name + ");\n";
		}
		else
		{
			GameServerDebug::AssertDebugMsg("파악할수 없는 타입이 체크되었습니다. Name : " + member_info.Name + " Type : " + member_info.Type);
		}
	}
}

void DeserializerTypeCheck(std::string& source, const MemberInfo& member_info)
{
	if (member_info.Type == "std::string")
	{
		source += "		serializer >> " + member_info.Name + ";\n";
	}
	else if (member_info.Type == "int")
	{
		source += "		serializer >> " + member_info.Name + ";\n";
	}
	else
	{
		if (member_info.Type[0] == 'E')
		{
			source += "		serializer.ReadEnum(" + member_info.Name + ");\n";
		}
		else
		{
			GameServerDebug::AssertDebugMsg("파악할수 없는 타입이 체크되었습니다. Name : " + member_info.Name + " Type : " + member_info.Type);
		}
	}
}

void CreateMessageHeader(std::vector<MessageInfo>& collection, std::string& source)
{
	source += "#pragma once																										\n";
	source += "#include \"GameServerMessage.h\"																					\n";
	source += "																													\n";

	for (auto& element : collection)
	{
		source += "class " + element.MessageName + "Message : public GameServerMessage											\n";
		source += "{																											\n";
		source += "public:																										\n";

		std::vector<MemberInfo>& member_list = element.MemberVariable;
		for (auto& member_element : member_list)
		{
			source += "	" + member_element.MemberText + ";\n";
		}

		source += "																												\n";
		source += "public:																										\n";
		source += "	" + element.MessageName + "Message()																		\n";
		source += "		: GameServerMessage(MessageType::" + element.MessageName + ")											\n";

		for (auto& member_element : member_list)
		{
			source += "		, " + member_element.Name + "()																		\n";
		}

		source += "	{																											\n";
		source += "	}																											\n";
		source += "	~" + element.MessageName + "Message() override = default;													\n";
		source += "																												\n";
		source += "	" + element.MessageName + "Message(const " + element.MessageName + "Message& other) = delete;				\n";
		source += "	" + element.MessageName + "Message(" + element.MessageName + "Message&& other) noexcept = delete;			\n";
		source += "																												\n";
		source += "	" + element.MessageName + "Message& operator=(const " + element.MessageName + "Message& other) = delete;	\n";
		source += "	" + element.MessageName + "Message& operator=(" + element.MessageName + "Message&& other) = delete;			\n";
		source += "																												\n";
		source += "public:																										\n";
		source += "	int SizeCheck() override																					\n";
		source += "	{																											\n";

		if (false == member_list.empty())
		{
			source += "		return ";

			for (size_t index = 0; index < member_list.size(); ++index)
			{
				source += "DataSizeCheck(" + member_list[index].Name + ")";
				source += index != member_list.size() - 1 ? " + " : ";\n";
			}
		}
		else
		{
			source += "		return 0;\n";
		}

		source += "	}																											\n";
		source += "																												\n";
		source += "	void Serialize(GameServerSerializer& serializer) override													\n";
		source += "	{																											\n";
		source += "		GameServerMessage::Serialize(serializer);																\n";
		source += "																												\n";

		for (auto& member_element : member_list)
		{
			SerializerTypeCheck(source, member_element);
		}

		source += "	}																											\n";
		source += "																												\n";
		source += "	void Deserialize(GameServerSerializer& serializer) override													\n";
		source += "	{																											\n";
		source += "		GameServerMessage::Deserialize(serializer);																\n";
		source += "																												\n";

		for (auto& member_element : member_list)
		{
			DeserializerTypeCheck(source, member_element);
		}

		source += "	}																											\n";
		source += "};																											\n\n";
	}

	//GameServerFile save_file = GameServerFile{ path, "wt" };
	//save_file.Write(source.c_str(), source.size());
}

void MessageReflection(std::vector<MessageInfo>& collection, const std::string& code)
{
	const std::vector<std::string> message_list = GameServerString::Split(code, '|');
	for (const auto& message_element : message_list)
	{
		if (true == message_element.empty())
		{
			continue;
		}
		
		std::vector<std::string> name_and_members = GameServerString::Split(message_element, '-');

		MessageInfo new_message;
		new_message.MessageName = name_and_members[0];

		if (1 >= name_and_members.size())
		{
			collection.push_back(new_message);
			continue;
		}

		std::vector<std::string> member_list = GameServerString::Split(name_and_members[1], ';');
		for (auto& member_element : member_list)
		{
			GameServerString::TextClear(member_element);
			if (true == member_element.empty())
			{
				continue;
			}

			std::vector<std::string> type_and_name = GameServerString::Split(member_element, ' ');

			MemberInfo new_member;
			new_member.MemberText = member_element;
			new_member.Type = type_and_name[0];
			new_member.Name = type_and_name[1];

			new_message.MemberVariable.push_back(new_member);
		}

		collection.push_back(new_message);
	}
}

int main()
{
	std::vector<MessageInfo> all_message;
	std::vector<MessageInfo> server_client_message;
	std::vector<MessageInfo> server_message;
	std::vector<MessageInfo> client_message;

	//						path		 text
	std::unordered_map<std::string, std::string> file_save_map;
	

	/////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////// Check Text File ////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////
	{
		GameServerDirectory file_dir;
		file_dir.MoveToParent("ProjectCode");
		file_dir.MoveToChild(R"(GameServerMessage\MessageInfo)");

		{
			GameServerFile load_file = { file_dir.AddFileNameToPath("ServerAndClient.txt"), "rt" };
			std::string file_code = load_file.GetString();
			MessageReflection(server_client_message, file_code);
		}

		{
			GameServerFile load_file = { file_dir.AddFileNameToPath("ServerToClient.txt"), "rt" };
			std::string file_code = load_file.GetString();
			MessageReflection(server_message, file_code);
		}

		{
			GameServerFile load_file = { file_dir.AddFileNameToPath("ClientToServer.txt"), "rt" };
			std::string file_code = load_file.GetString();
			MessageReflection(client_message, file_code);
		}

		all_message.reserve(server_client_message.size() + server_message.size() + client_message.size());
		for (const auto& server_client_element : server_client_message)
		{
			all_message.emplace_back(server_client_element);
		}

		for (const auto& server_element : server_message)
		{
			all_message.emplace_back(server_element);
		}

		for (const auto& client_element : client_message)
		{
			all_message.emplace_back(client_element);
		}
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////// Check End ///////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////



	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////// Generate Server File ////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	{
		{
			/////////////////////////////////////// Create Enum File ///////////////////////////////////////
			{
				GameServerDirectory enum_file_dir;
				enum_file_dir.MoveToParent("ProjectCode");
				enum_file_dir.MoveToChild("GameServerMessage");

				std::string enum_file_text;
				enum_file_text += "#pragma once\n";
				enum_file_text += "\n";
				enum_file_text += "enum class MessageType\n";
				enum_file_text += "{\n";

				for (const auto& message_element : all_message)
				{
					enum_file_text += "\t" + message_element.MessageName + ",\n";
				}

				enum_file_text += "};";

				//GameServerFile save_file = { enum_file_dir.AddFileNameToPath("MessageTypeEnum.h"), "wt" };
				//save_file.Write(enum_file_text.c_str(), enum_file_text.size());

				file_save_map.insert(make_pair(enum_file_dir.AddFileNameToPath("MessageTypeEnum.h"), enum_file_text));
			}
			////////////////////////////////////////////////////////////////////////////////////////////////

			///////////////////////////////////// Create Convert File //////////////////////////////////////
			{
				GameServerDirectory convert_file_dir;
				convert_file_dir.MoveToParent("ProjectCode");
				convert_file_dir.MoveToChild("GameServerMessage");

				std::string convert_file_text;
				convert_file_text += "#include \"PreCompile.h\"														\n";
				convert_file_text += "#include \"MessageConverter.h\"												\n";
				convert_file_text += "																				\n";
				convert_file_text += "MessageConverter::MessageConverter(const std::vector<unsigned char>& buffer)	\n";
				convert_file_text += "	: m_Buffer(buffer)															\n";
				convert_file_text += "{																				\n";
				convert_file_text += "	GameServerSerializer serializer = GameServerSerializer(buffer);				\n";
				convert_file_text += "	MessageType type;															\n";
				convert_file_text += "																				\n";
				convert_file_text += "	memcpy_s(&type, sizeof(MessageType), &buffer[0], sizeof(MessageType));		\n";
				convert_file_text += "	switch (type)																\n";
				convert_file_text += "	{																			\n";

				for (const auto& message_element : all_message)
				{
					convert_file_text += "	case MessageType::" + message_element.MessageName + ":\n";
					convert_file_text += "		m_Message = std::make_shared<" + message_element.MessageName + "Message>();\n";
					convert_file_text += "		break;\n";
				}

				convert_file_text += "	default:																	\n";
				convert_file_text += "		return;																	\n";
				convert_file_text += "	}																			\n";
				convert_file_text += "																				\n";
				convert_file_text += "	if (nullptr == m_Message)													\n";
				convert_file_text += "	{																			\n";
				convert_file_text += "		return;																	\n";
				convert_file_text += "	}																			\n";
				convert_file_text += "																				\n";
				convert_file_text += "	m_Message->Deserialize(serializer);											\n";
				convert_file_text += "}";

				//GameServerFile save_file = { convert_file_dir.AddFileNameToPath("MessageConverter.cpp"), "wt" };
				//save_file.Write(convert_file_text.c_str(), convert_file_text.size());

				file_save_map.insert(make_pair(convert_file_dir.AddFileNameToPath("MessageConverter.cpp"), convert_file_text));
			}
			////////////////////////////////////////////////////////////////////////////////////////////////
		}

		{
			//////////////////////////////////////// Message Header ////////////////////////////////////////
			{
				GameServerDirectory header_file_dir;
				header_file_dir.MoveToParent("ProjectCode");
				header_file_dir.MoveToChild("GameServerMessage");

				std::string server_client_text;
				std::string server_text;
				std::string client_text;

				CreateMessageHeader(server_client_message, server_client_text);
				CreateMessageHeader(server_message, server_text);
				CreateMessageHeader(client_message, client_text);

				file_save_map.insert(make_pair(header_file_dir.AddFileNameToPath("ServerAndClient.h"), server_client_text));
				file_save_map.insert(make_pair(header_file_dir.AddFileNameToPath("ServerToClient.h"), server_text));
				file_save_map.insert(make_pair(header_file_dir.AddFileNameToPath("ClientToServer.h"), client_text));
			}
			////////////////////////////////////////////////////////////////////////////////////////////////

			/////////////////////////////////// Create Server Dispatcher ///////////////////////////////////
			{
				GameServerDirectory dispatcher_file_dir;
				dispatcher_file_dir.MoveToParent("ProjectCode");
				dispatcher_file_dir.MoveToChild("GameServerApp");

				std::string dispatcher_text;

				dispatcher_text += "#include \"PreCompile.h\"																													\n";
				dispatcher_text += "#include \"ServerDispatcher.h\"																												\n";
				dispatcher_text += "#include <GameServerBase/GameServerDebug.h>																									\n";
				dispatcher_text += "																																			\n";
				dispatcher_text += "#include \"ThreadHandlerLoginMessage.h\"																									\n";
				dispatcher_text += "#include \"ThreadHandlerChatMessage.h\"																										\n";
				dispatcher_text += "																																			\n";
				dispatcher_text += "Dispatcher<TCPSession> g_dispatcher;																										\n";
				dispatcher_text += "																																			\n";
				dispatcher_text += "template <typename MessageHandler, typename MessageType>																					\n";
				dispatcher_text += "void OnMessageProcess(std::shared_ptr<TCPSession> tcp_session, std::shared_ptr<GameServerMessage> message)									\n";
				dispatcher_text += "{																																			\n";
				dispatcher_text += "	std::shared_ptr<MessageType> convert_message = std::dynamic_pointer_cast<MessageType>(std::move(message));								\n";
				dispatcher_text += "	if (nullptr == convert_message)																											\n";
				dispatcher_text += "	{																																		\n";
				dispatcher_text += "		GameServerDebug::LogError(\"MessageConvert Error\");																				\n";
				dispatcher_text += "		return;																																\n";
				dispatcher_text += "	}																																		\n";
				dispatcher_text += "																																			\n";
				dispatcher_text += "	std::shared_ptr<MessageHandler> message_handler = std::make_shared<MessageHandler>(std::move(tcp_session), std::move(convert_message));	\n";
				dispatcher_text += "	message_handler->Start();																												\n";
				dispatcher_text += "}																																			\n";
				dispatcher_text += "																																			\n";
				dispatcher_text += "void DispatcherRegistration()																												\n";
				dispatcher_text += "{																																			";

				for (auto& server_client_element : server_client_message)
				{
					dispatcher_text += "																												\n";
					dispatcher_text += "		g_dispatcher.AddHandler(static_cast<uint32_t>(MessageType::" + server_client_element.MessageName + "),	\n";
					dispatcher_text += "		[](std::shared_ptr<TCPSession> tcp_session, std::shared_ptr<GameServerMessage> message)					\n";
					dispatcher_text += "		{																										\n";
					dispatcher_text += "			return OnMessageProcess<ThreadHandler" + server_client_element.MessageName + "Message, " +
													server_client_element.MessageName + "Message>(std::move(tcp_session), std::move(message));			\n";
					dispatcher_text += "		});																										\n";
				}

				for (auto& client_element : client_message)
				{
					dispatcher_text += "																												\n";
					dispatcher_text += "		g_dispatcher.AddHandler(static_cast<uint32_t>(MessageType::" + client_element.MessageName + "),			\n";
					dispatcher_text += "		[](std::shared_ptr<TCPSession> tcp_session, std::shared_ptr<GameServerMessage> message)					\n";
					dispatcher_text += "		{																										\n";
					dispatcher_text += "			return OnMessageProcess<ThreadHandler" + client_element.MessageName + "Message, " +
													client_element.MessageName + "Message>(std::move(tcp_session), std::move(message));					\n";
					dispatcher_text += "		});																										\n";
				}

				dispatcher_text += "}																																		     ";

				//GameServerFile save_file = { dispatcher_file_dir.AddFileNameToPath("ServerDispatcher.cpp"), "wt" };
				//save_file.Write(dispatcher_text.c_str(), dispatcher_text.size());

				file_save_map.insert(make_pair(dispatcher_file_dir.AddFileNameToPath("ServerDispatcher.cpp"), dispatcher_text));
			}
			////////////////////////////////////////////////////////////////////////////////////////////////
		}
	}

	for (const auto& map_element : file_save_map)
	{
		GameServerFile save_file = { map_element.first, "wt" };
		save_file.Write(map_element.second.c_str(), map_element.second.size());
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////// End Server File ///////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////





	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////// Generate Unreal File ////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	{
		
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////// End Unreal File ///////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	return 0;







	// 단순복사
	{
		GameServerDirectory file_dir;
		file_dir.MoveToParent("ProjectCode");
		file_dir.MoveToChild("GameServerBase");

		GameServerDirectory save_dir;
		save_dir.MoveToParent("ProjectCode");
		save_dir.MoveToParent();
		save_dir.MoveToChild(UNREAL_CLIENT_DIR);


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