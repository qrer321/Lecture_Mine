#include <GameServerBase/GameServerDebug.h>
#include <GameServerBase/GameServerFile.h>
#include <GameServerBase/GameServerString.h>
#include <GameServerBase/GameServerDirectory.h>
#include <unordered_map>
#include <sstream>
#include <tinyxml2.h>

#pragma comment (lib, "GameServerBase.lib")
#pragma comment (lib, "tinyxml2.lib")

// 폴더에 따라 변경해서 사용
#define UNREAL_CLIENT_MESSAGE_DIR "HonorEtude\\Source\\HonorProject\\Message"

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
	else if (member_info.Type == "__int64")
	{
		source += "		serializer << " + member_info.Name + ";\n";
	}
	else if (member_info.Type == "uint64_t")
	{
		source += "		serializer << " + member_info.Name + ";\n";
	}
	else if (member_info.Type == "FVector4")
	{
		source += "		serializer << " + member_info.Name + ";\n";
	}
	else if (std::string::npos != member_info.Type.find("std::vector"))
	{
		source += "		serializer.WriteVector(" + member_info.Name + ");\n";
	}
	else
	{
		if (member_info.Type[0] == 'E')
		{
			source += "		serializer.WriteEnum(" + member_info.Name + ");\n";
		}
		else if (member_info.Type[0] == 'F')
		{
			source += "		" + member_info.Name + ".Serialize(serializer);\n";
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
	else if (member_info.Type == "__int64")
	{
		source += "		serializer >> " + member_info.Name + ";\n";
	}
	else if (member_info.Type == "uint64_t")
	{
		source += "		serializer >> " + member_info.Name + ";\n";
	}
	else if (member_info.Type == "FVector4")
	{
		source += "		serializer >> " + member_info.Name + ";\n";
	}
	else if (std::string::npos != member_info.Type.find("std::vector"))
	{
		source += "		serializer.ReadVector(" + member_info.Name + ");\n";
	}
	else
	{
		if (member_info.Type[0] == 'E')
		{
			source += "		serializer.ReadEnum(" + member_info.Name + ");\n";
		}
		else if (member_info.Type[0] == 'F')
		{
			source += "		" + member_info.Name + ".DeSerialize(serializer);\n";
		}
		else
		{
			GameServerDebug::AssertDebugMsg("파악할수 없는 타입이 체크되었습니다. Name : " + member_info.Name + " Type : " + member_info.Type);
		}
	}
}

void CreateMessageHeader(std::vector<MessageInfo>& collection, std::string& source, const std::string& include)
{
	source += "#pragma once																										\n";
	source += "#include <GameServerMessage/GameServerMessage.h>																	\n";
	source += include;
	source += "																													\n";

	for (MessageInfo& element : collection)
	{
		source += "class " + element.MessageName + "Message : public GameServerMessage											\n";
		source += "{																											\n";
		source += "public:																										\n";

		std::vector<MemberInfo>& member_list = element.MemberVariable;
		for (MemberInfo& member_element : member_list)
		{
			source += "	" + member_element.MemberText + ";\n";
		}

		source += "																												\n";
		source += "public:																										\n";
		source += "	" + element.MessageName + "Message()																		\n";
		source += "		: GameServerMessage(static_cast<uint32_t>(MessageType::" + element.MessageName + "))					\n";

		for (MemberInfo& member_element : member_list)
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
}

void MessageReflection(std::vector<MessageInfo>& collection, const std::string& code)
{
	const std::vector<std::string> message_list = GameServerString::Split(code, '|');
	for (const std::string& message_element : message_list)
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
		for (std::string& member_element : member_list)
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
	GameServerDirectory directory;
	directory.MoveToParent("Lecture_Mine");
	directory.MoveToChild("ProjectCode");
	directory.MoveToChild("MessageToUnreal");

	std::stringstream xml_stream;
	xml_stream << directory.AddFileNameToPath("MessageConfig.xml");

	tinyxml2::XMLDocument xml_document;
	xml_document.LoadFile(xml_stream.str().c_str());
	if (true == xml_document.Error())
	{
		GameServerDebug::LogErrorAssert("Open Error From MessageConfig.xml");
		return 1;
	}

	tinyxml2::XMLElement* info_element = xml_document.FirstChildElement("MessageParserInfo");
	if (nullptr == info_element)
	{
		GameServerDebug::LogErrorAssert("MessageParserInfo Element Is Missing In XML Document");
		return 1;
	}

	std::string include_element;

	{
		// <ServerPort Port = "30001" / >
		tinyxml2::XMLElement* message_element = info_element->FirstChildElement("MessageFileOption");
		include_element = nullptr != message_element->FindAttribute("Include") ? message_element->FindAttribute("Include")->Value() : "";

		std::vector<std::string> message_string = GameServerString::Split(include_element, ',');

		include_element.clear();
		for (const auto& element : message_string)
		{
			include_element += "#include \"" + element + "\"\n";
		}
	}

	std::vector<MessageInfo> all_message;
	std::vector<MessageInfo> server_client_message;
	std::vector<MessageInfo> server_message;
	std::vector<MessageInfo> client_message;

	//						path		 text
	std::unordered_map<std::string, std::string> server_save_map;
	std::unordered_map<std::string, std::string> client_save_map;
	

	/////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////// Check Text File ////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////
	{
		GameServerDirectory file_dir;
		if (false == file_dir.MoveToParent("ProjectCode"))
		{
			return 1;
		}
		if (false == file_dir.MoveToChild("GameServerContents\\Info"))
		{
			return 1;
		}

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
				if (false == enum_file_dir.MoveToParent("ProjectCode"))
				{
					return 1;
				}
				if (false == enum_file_dir.MoveToChild("GameServerContents"))
				{
					return 1;
				}

				std::string enum_file_text;
				enum_file_text += "#pragma once\n";
				enum_file_text += "\n";
				enum_file_text += "enum class MessageType\n";
				enum_file_text += "{\n";

				for (const MessageInfo& message_element : all_message)
				{
					enum_file_text += "\t" + message_element.MessageName + ",\n";
				}

				enum_file_text += "\t";
				enum_file_text += "MAX";
				enum_file_text += "\n";
				enum_file_text += "};";

				server_save_map.insert(make_pair(enum_file_dir.AddFileNameToPath("MessageTypeEnum.h"), enum_file_text));
			}
			////////////////////////////////////////////////////////////////////////////////////////////////

			///////////////////////////////////// Create Convert File //////////////////////////////////////
			{
				GameServerDirectory convert_file_dir;
				if (false == convert_file_dir.MoveToParent("ProjectCode"))
				{
					return 1;
				}
				if (false == convert_file_dir.MoveToChild("GameServerContents"))
				{
					return 1;
				}

				std::string convert_file_text;
				convert_file_text += "#include \"PreCompile.h\"														\n";
				convert_file_text += "#include <GameServerMessage/MessageConverter.h>								\n";
				convert_file_text += "#include \"MessageTypeEnum.h\"												\n";
				convert_file_text += "#include \"ServerAndClient.h\"												\n";
				convert_file_text += "#include \"ServerToClient.h\"													\n";
				convert_file_text += "#include \"ClientToServer.h\"													\n";
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

				server_save_map.insert(make_pair(convert_file_dir.AddFileNameToPath("MessageConverter.cpp"), convert_file_text));
			}
			////////////////////////////////////////////////////////////////////////////////////////////////
		}

		{
			//////////////////////////////////////// Message Header ////////////////////////////////////////
			{
				GameServerDirectory header_file_dir;
				if (false == header_file_dir.MoveToParent("ProjectCode"))
				{
					return 1;
				}
				if (false == header_file_dir.MoveToChild("GameServerContents"))
				{
					return 1;
				}

				std::string server_client_text;
				std::string server_text;
				std::string client_text;

				CreateMessageHeader(server_client_message, server_client_text, include_element);
				CreateMessageHeader(server_message, server_text, include_element);
				CreateMessageHeader(client_message, client_text, include_element);

				server_save_map.insert(make_pair(header_file_dir.AddFileNameToPath("ServerAndClient.h"), server_client_text));
				server_save_map.insert(make_pair(header_file_dir.AddFileNameToPath("ServerToClient.h"), server_text));
				server_save_map.insert(make_pair(header_file_dir.AddFileNameToPath("ClientToServer.h"), client_text));
			}
			////////////////////////////////////////////////////////////////////////////////////////////////

			/////////////////////////////////// Create Server Dispatcher ///////////////////////////////////
			{
				GameServerDirectory dispatcher_file_dir;
				if (false == dispatcher_file_dir.MoveToParent("ProjectCode"))
				{
					return 1;
				}
				if (false == dispatcher_file_dir.MoveToChild("GameServerContents"))
				{
					return 1;
				}

				std::string dispatcher_text;

				dispatcher_text += "#include \"PreCompile.h\"																													\n";
				dispatcher_text += "#include \"ServerDispatcher.h\"																												\n";
				dispatcher_text += "#include <GameServerBase/GameServerDebug.h>																									\n";
				dispatcher_text += "																																			\n";

				for (const MessageInfo& server_client_element : server_client_message)
				{
					dispatcher_text += "#include \"ThreadHandler" + server_client_element.MessageName + "Message.h\"															\n";
				}

				for (const MessageInfo& client_element : client_message)
				{
					dispatcher_text += "#include \"ThreadHandler" + client_element.MessageName + "Message.h\"																	\n";
				}

				dispatcher_text += "																																			\n";
				dispatcher_text += "Dispatcher<TCPSession> g_tcp_dispatcher;																									\n";
				dispatcher_text += "Dispatcher<UDPSession> g_udp_dispatcher;																									\n";
				dispatcher_text += "																																			\n";
				dispatcher_text += "template <typename MessageHandler, typename MessageType, typename SessionType = TCPSession>													\n";
				dispatcher_text += "void OnMessageProcess(std::shared_ptr<SessionType> session, std::shared_ptr<GameServerMessage> message)										\n";
				dispatcher_text += "{																																			\n";
				dispatcher_text += "	std::shared_ptr<MessageType> convert_message = std::dynamic_pointer_cast<MessageType>(std::move(message));								\n";
				dispatcher_text += "	if (nullptr == convert_message)																											\n";
				dispatcher_text += "	{																																		\n";
				dispatcher_text += "		GameServerDebug::LogError(\"MessageConvert Error\");																				\n";
				dispatcher_text += "		return;																																\n";
				dispatcher_text += "	}																																		\n";
				dispatcher_text += "																																			\n";
				dispatcher_text += "	std::shared_ptr<MessageHandler> message_handler = std::make_shared<MessageHandler>();													\n";
				dispatcher_text += "	message_handler->Init(std::move(session), std::move(convert_message));																\n";
				dispatcher_text += "	message_handler->Start();																												\n";
				dispatcher_text += "}																																			\n";
				dispatcher_text += "																																			\n";
				dispatcher_text += "void DispatcherRegistration()																												\n";
				dispatcher_text += "{																																			";

				for (auto& server_client_element : server_client_message)
				{
					dispatcher_text += "																													\n";
					//dispatcher_text += "		g_tcp_dispatcher.AddHandler(static_cast<uint32_t>(MessageType::" + server_client_element.MessageName + "),	\n";
					//dispatcher_text += "		[](std::shared_ptr<TCPSession> tcp_session, std::shared_ptr<GameServerMessage> message)						\n";
					//dispatcher_text += "		{																											\n";
					//dispatcher_text += "			OnMessageProcess<ThreadHandler" + server_client_element.MessageName + "Message, " +
					//								server_client_element.MessageName + "Message + "" +>(std::move(tcp_session), std::move(message));		\n";
					//dispatcher_text += "		});																											\n";
					dispatcher_text += "		g_tcp_dispatcher.AddHandler(static_cast<uint32_t>(MessageType::" + server_client_element.MessageName + "), " + 
												"std::bind(&OnMessageProcess<ThreadHandler" + server_client_element.MessageName + "Message, " +
												server_client_element.MessageName + "Message>, std::placeholders::_1, std::placeholders::_2));				\n";
				}

				for (auto& client_element : client_message)
				{
					dispatcher_text += "																													\n";
					//dispatcher_text += "		g_tcp_dispatcher.AddHandler(static_cast<uint32_t>(MessageType::" + client_element.MessageName + "),			\n";
					//dispatcher_text += "		[](std::shared_ptr<TCPSession> tcp_session, std::shared_ptr<GameServerMessage> message)						\n";
					//dispatcher_text += "		{																											\n";
					//dispatcher_text += "			OnMessageProcess<ThreadHandler" + client_element.MessageName + "Message, " +
					//								client_element.MessageName + "Message>(std::move(tcp_session), std::move(message));						\n";
					//dispatcher_text += "		});																											\n";
					dispatcher_text += "		g_tcp_dispatcher.AddHandler(static_cast<uint32_t>(MessageType::" + client_element.MessageName + "), " +
												"std::bind(&OnMessageProcess<ThreadHandler" + client_element.MessageName + "Message, " +
												client_element.MessageName + "Message>, std::placeholders::_1, std::placeholders::_2));						\n";
				}

				dispatcher_text += "}																														";

				server_save_map.insert(make_pair(dispatcher_file_dir.AddFileNameToPath("ServerDispatcher.cpp"), dispatcher_text));
			}
			////////////////////////////////////////////////////////////////////////////////////////////////
		}
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////// End Server File //////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////


	for (const std::pair<const std::string, std::string>& save_element : server_save_map)
	{
		GameServerFile save_file = { save_element.first, "wt" };
		save_file.Write(save_element.second.c_str(), save_element.second.size());
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////// Generate Unreal File ////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	{
		/////////////////////////////////////// Copy GameServerBase //////////////////////////////////////
		{
			GameServerDirectory server_base_dir;
			if (false == server_base_dir.MoveToParent("ProjectCode"))
			{
				return 1;
			}
			if (false == server_base_dir.MoveToChild("GameServerBase"))
			{
				return 1;
			}

			GameServerDirectory save_dir;
			//save_dir.MoveToRootDirectory();
			save_dir.MoveToParent();
			save_dir.MoveToParent();
			if (false == save_dir.MoveToChild(UNREAL_CLIENT_MESSAGE_DIR))
			{
				return 1;
			}

			/////////////////////////////////////// Copy GameServerSerializer //////////////////////////////////////
			{
				{
					GameServerFile load_file = { server_base_dir.AddFileNameToPath("GameServerSerializer.h"), "rt" };
					std::string file_code = load_file.GetString();

					if (std::string::size_type find_pos; std::string::npos != (find_pos = file_code.find("#include \"GameServerMathStruct.h\"\n")))
					{
						file_code.replace(find_pos, strlen("#include \"GameServerMathStruct.h\"\n"), "\n");
					}
					
					client_save_map.insert(make_pair(save_dir.AddFileNameToPath("GameServerSerializer.h"), file_code));
				}

				{
					GameServerFile load_file = { server_base_dir.AddFileNameToPath("GameServerSerializer.cpp"), "rt" };
					std::string file_code = load_file.GetString();

					if (std::string::size_type find_pos; std::string::npos != (find_pos = file_code.find("#include \"PreCompile.h\"")))
					{
						file_code.erase(find_pos, strlen("#include \"PreCompile.h\"") + 1);
					}

					client_save_map.insert(make_pair(save_dir.AddFileNameToPath("GameServerSerializer.cpp"), file_code));
				}
			}
			////////////////////////////////////////////////////////////////////////////////////////////////////////
		}
		//////////////////////////////////////////////////////////////////////////////////////////////////
		
		////////////////////////////////////// Copy GameServerMessage ////////////////////////////////////
		{
			{
				GameServerDirectory server_message_dir;
				if (false == server_message_dir.MoveToParent("ProjectCode"))
				{
					return 1;
				}
				if (false == server_message_dir.MoveToChild("GameServerMessage"))
				{
					return 1;
				}

				GameServerDirectory contents_message_dir;
				if (false == contents_message_dir.MoveToParent("ProjectCode"))
				{
					return 1;
				}
				if (false == contents_message_dir.MoveToChild("GameServerContents"))
				{
					return 1;
				}

				GameServerDirectory save_dir;
				//save_dir.MoveToRootDirectory();
				save_dir.MoveToParent();
				save_dir.MoveToParent();
				if (false == save_dir.MoveToChild(UNREAL_CLIENT_MESSAGE_DIR))
				{
					return 1;
				}

				///////////////////////////////////// Copy Messages ////////////////////////////////////////
				{
					{
						GameServerFile load_file = { server_message_dir.AddFileNameToPath("Messages.h"), "rt" };
						std::string file_code = load_file.GetString();

						client_save_map.insert(make_pair(save_dir.AddFileNameToPath("Messages.h"), file_code));
					}

					{
						GameServerFile load_file = { server_message_dir.AddFileNameToPath("GameServerMessage.h"), "rt" };
						std::string file_code = load_file.GetString();

						if (std::string::size_type find_pos; std::string::npos != (find_pos = file_code.find("#include <GameServerBase/GameServerSerializer.h>\n")))
						{
							file_code.replace(find_pos, strlen("#include <GameServerBase/GameServerSerializer.h>\n"), "#include \"GameServerSerializer.h\"\n");
						}
						
						if (std::string::size_type find_pos; std::string::npos != (find_pos = file_code.find("#include <GameServerBase/GameServerMathStruct.h>\n")))
						{
							file_code.replace(find_pos, strlen("#include <GameServerBase/GameServerMathStruct.h>\n"), "\n");
						}

						client_save_map.insert(make_pair(save_dir.AddFileNameToPath("GameServerMessage.h"), file_code));
					}

					{
						GameServerFile load_file = { contents_message_dir.AddFileNameToPath("ServerAndClient.h"), "rt" };
						std::string file_code = load_file.GetString();

						if (std::string::size_type find_pos; std::string::npos != (find_pos = file_code.find("#include <GameServerMessage/GameServerMessage.h>")))
						{
							file_code.replace(find_pos, strlen("#include <GameServerMessage/GameServerMessage.h>"), "#include \"GameServerMessage.h\"");
						}

						client_save_map.insert(make_pair(save_dir.AddFileNameToPath("ServerAndClient.h"), file_code));
					}

					{
						GameServerFile load_file = { contents_message_dir.AddFileNameToPath("ServerToClient.h"), "rt" };
						std::string file_code = load_file.GetString();

						if (std::string::size_type find_pos; std::string::npos != (find_pos = file_code.find("#include <GameServerMessage/GameServerMessage.h>")))
						{
							file_code.replace(find_pos, strlen("#include <GameServerMessage/GameServerMessage.h>"), "#include \"GameServerMessage.h\"");
						}

						client_save_map.insert(make_pair(save_dir.AddFileNameToPath("ServerToClient.h"), file_code));
					}

					{
						GameServerFile load_file = { contents_message_dir.AddFileNameToPath("ClientToServer.h"), "rt" };
						std::string file_code = load_file.GetString();

						if (std::string::size_type find_pos; std::string::npos != (find_pos = file_code.find("#include <GameServerMessage/GameServerMessage.h>")))
						{
							file_code.replace(find_pos, strlen("#include <GameServerMessage/GameServerMessage.h>"), "#include \"GameServerMessage.h\"");
						}

						client_save_map.insert(make_pair(save_dir.AddFileNameToPath("ClientToServer.h"), file_code));
					}
				}
				////////////////////////////////////////////////////////////////////////////////////////////

				///////////////////////////////////// Copy MessageTypeEnum /////////////////////////////////
				{
					GameServerFile load_file = { contents_message_dir.AddFileNameToPath("MessageTypeEnum.h"), "rt" };
					std::string file_code = load_file.GetString();

					client_save_map.insert(make_pair(save_dir.AddFileNameToPath("MessageTypeEnum.h"), file_code));
				}
				////////////////////////////////////////////////////////////////////////////////////////////
				
				///////////////////////////////////// Copy ContentsEnum ////////////////////////////////////
				{
					GameServerFile load_file = { contents_message_dir.AddFileNameToPath("ContentsEnum.h"), "rt" };
					std::string file_code = load_file.GetString();

					client_save_map.insert(make_pair(save_dir.AddFileNameToPath("ContentsEnum.h"), file_code));
				}
				////////////////////////////////////////////////////////////////////////////////////////////

				/////////////////////////////////// Copy ContentsStructure //////////////////////////////////
				{
					GameServerFile load_file = { contents_message_dir.AddFileNameToPath("ContentsStructure.h"), "rt" };
					std::string file_code = load_file.GetString();

					if (std::string::size_type find_pos; std::string::npos != (find_pos = file_code.find("#include <GameServerBase/GameServerSerializer.h>\n")))
					{
						file_code.replace(find_pos, strlen("#include <GameServerBase/GameServerSerializer.h>\n"), "#include \"GameServerSerializer.h\"\n");
					}

					client_save_map.insert(make_pair(save_dir.AddFileNameToPath("ContentsStructure.h"), file_code));
				}
				////////////////////////////////////////////////////////////////////////////////////////////

				///////////////////////////////////// Copy MessageConverter ////////////////////////////////
				{
					{
						GameServerFile load_file = { server_message_dir.AddFileNameToPath("MessageConverter.h"), "rt" };
						std::string file_code = load_file.GetString();

						client_save_map.insert(make_pair(save_dir.AddFileNameToPath("MessageConverter.h"), file_code));
					}

					{
						GameServerFile load_file = { contents_message_dir.AddFileNameToPath("MessageConverter.cpp"), "rt" };
						std::string file_code = load_file.GetString();

						file_code.erase(0, strlen("#include \"PreCompile.h\"") + 1);

						if (std::string::size_type find_pos; std::string::npos != (find_pos = file_code.find("#include <GameServerMessage/MessageConverter.h>")))
						{
							file_code.replace(find_pos, strlen("#include <GameServerMessage/MessageConverter.h>"), "#include \"MessageConverter.h\"");
						}

						client_save_map.insert(make_pair(save_dir.AddFileNameToPath("MessageConverter.cpp"), file_code));
					}
				}
				////////////////////////////////////////////////////////////////////////////////////////////
				
				///////////////////////////////////// Create Client Dispatcher//////////////////////////////
				{
					std::string dispatcher_text;

					dispatcher_text += "#pragma once																															 \n";
					dispatcher_text += "																																		 \n";

					for (const MessageInfo& server_client_element : server_client_message)
					{
						dispatcher_text += "#include \"ThreadHandler" + server_client_element.MessageName + "Message.h\"														\n";
					}

					for (const MessageInfo& server_element : server_message)
					{
						dispatcher_text += "#include \"ThreadHandler" + server_element.MessageName + "Message.h\"																\n";
					}

					dispatcher_text += "																																		 \n";
					dispatcher_text += "template <typename MessageHandler, typename MessageType>																				 \n";
					dispatcher_text += "void OnMessageProcess(std::shared_ptr<GameServerMessage> Message, UWorld* World)														 \n";
					dispatcher_text += "{																																		 \n";
					dispatcher_text += "	std::shared_ptr<MessageType> ConvertMessage = std::static_pointer_cast<MessageType>(MoveTemp(Message));								 \n";
					dispatcher_text += "	if (nullptr == ConvertMessage)																										 \n";
					dispatcher_text += "	{																																	 \n";
					dispatcher_text += "		return;																															 \n";
					dispatcher_text += "	}																																	 \n";
					dispatcher_text += "																																		 \n";
					dispatcher_text += "	UHonorProjectGameInstance* GameInstance = Cast<UHonorProjectGameInstance>(World->GetGameInstance());								 \n";
					dispatcher_text += "																																		 \n";
					dispatcher_text += "	MessageHandler Handler = MessageHandler();																							 \n";
					dispatcher_text += "	Handler.Initialize(MoveTempIfPossible(ConvertMessage), GameInstance, World);														 \n";
					dispatcher_text += "	Handler.Start();																													 \n";
					dispatcher_text += "}																																		 \n";
					dispatcher_text += "																																		 \n";
					dispatcher_text += "inline void AddGlobalHandler(Dispatcher& Dis, UWorld* World)																			 \n";
					dispatcher_text += "{																																		   ";

					for (const MessageInfo& server_client_element : server_client_message)
					{
						dispatcher_text += "\n";
						dispatcher_text += "	Dis.AddHandler(MessageType::" + server_client_element.MessageName + ",\n";
						dispatcher_text += "	               [World](std::shared_ptr<GameServerMessage> GameServerMessage)\n";
						dispatcher_text += "	               {\n";
						dispatcher_text += "		               OnMessageProcess<ThreadHandler" + server_client_element.MessageName
																+ "Message, " + server_client_element.MessageName
																+ "Message>(MoveTemp(GameServerMessage), World);\n";
						dispatcher_text += "	               });\n";
					}

					for (const MessageInfo& server_element : server_message)
					{
						dispatcher_text += "																																	\n";
						dispatcher_text += "	Dis.AddHandler(MessageType::" + server_element.MessageName + ",																	\n";
						dispatcher_text += "	               [World](std::shared_ptr<GameServerMessage> GameServerMessage)													\n";
						dispatcher_text += "	               {																												\n";
						dispatcher_text += "		               OnMessageProcess<ThreadHandler" + server_element.MessageName
																	+ "Message, " + server_element.MessageName
																	+ "Message>(MoveTemp(GameServerMessage), World);															\n";
						dispatcher_text += "	               });																												\n";
					}

					dispatcher_text += "}																																  		 \n";

					client_save_map.insert(make_pair(save_dir.AddFileNameToPath("Handler\\HandlerHeader.h"), dispatcher_text));
				}
				////////////////////////////////////////////////////////////////////////////////////////////
			}
		}
		//////////////////////////////////////////////////////////////////////////////////////////////////
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////// End Unreal File //////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////

	for (const std::pair<const std::string, std::string>& save_element : client_save_map)
	{
		GameServerFile save_file = { save_element.first, "wt" };
		save_file.Write(save_element.second.c_str(), save_element.second.size());
	}

	return 0;
}