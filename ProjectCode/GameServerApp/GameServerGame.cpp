#include "PreCompile.h"
#include "GameServerGame.h"
#include <GameServerBase/GameServerDebug.h>
#include <GameServerBase/GameServerDirectory.h>

GameServerGame* GameServerGame::m_Inst = new GameServerGame();

void GameServerGame::Destroy()
{
	if (nullptr != m_Inst)
	{
		delete m_Inst;
		m_Inst = nullptr;
	}
}

bool GameServerGame::Start()
{
	GameServerDirectory directory;
	directory.MoveToParent("ProjectCode");

	std::stringstream xml_stream;
	xml_stream << directory.AddFileNameToPath("ServerConfig.xml");

	tinyxml2::XMLDocument xml_document;
	xml_document.LoadFile(xml_stream.str().c_str());
	if (true == xml_document.Error())
	{
		GameServerDebug::LogErrorAssert("Open Error From ServerConfig.xml");
		return false;
	}

	tinyxml2::XMLElement* root_element = xml_document.FirstChildElement("ServerApp");
	if (nullptr == root_element)
	{
		GameServerDebug::LogErrorAssert("ServerApp Element Is Missing In XML Document");
		return false;
	}

	{
		// <ServerPort Port = "30001" / >
		tinyxml2::XMLElement* server_element = root_element->FirstChildElement("ServerPort");
		const tinyxml2::XMLAttribute* server_port = server_element->FindAttribute("Port");

		std::string_view port = server_port->Value();
	}

	{
		// <DataBase Host="127.0.0.1" Port="3306" Name="userver2" User="root" PW="1234" />
		tinyxml2::XMLElement* db_element = root_element->FirstChildElement("DataBase");

		std::string_view host = db_element->FindAttribute("Host")->Value();
		std::string_view port = db_element->FindAttribute("Port")->Value();
		std::string_view name = db_element->FindAttribute("Name")->Value();
		std::string_view user = db_element->FindAttribute("User")->Value();
		std::string_view pw  = db_element->FindAttribute("PW")->Value();
	}

	return true;
}

bool GameServerGame::Run()
{
	return true;
}
