#include "PreCompile.h"
#include "GameServerCore.h"
#include <GameServerBase/GameServerDebug.h>
#include <GameServerBase/GameServerDirectory.h>
#include <GameServerNet/ServerHelper.h>
#include "DBQueue.h"
#include "NetQueue.h"

int			GameServerCore::s_ServerPort;
int			GameServerCore::s_DBPort;
std::string	GameServerCore::s_DBHost;
std::string	GameServerCore::s_DBSchema;
std::string	GameServerCore::s_DBUser;
std::string	GameServerCore::s_DBPw;

bool GameServerCore::CoreDataCheck()
{
	if (-1 == s_ServerPort || -1 == s_DBPort)
	{
		return false;
	}

	if (s_DBHost.empty() || s_DBSchema.empty() || s_DBUser.empty() || s_DBPw.empty())
	{
		return false;
	}

	return true;
}

bool GameServerCore::CoreInit()
{
	GameServerDebug::Initialize();
	GameServerDebug::LogInfo("Server Initialize Start");

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
		s_ServerPort = nullptr != server_element->FindAttribute("Port") ? std::stoi(server_element->FindAttribute("Port")->Value()) : -1;

		GameServerDebug::LogInfo("Server Config Info");
		GameServerDebug::LogInfo(std::string("ServerPort : " + std::to_string(s_ServerPort)));
	}

	{
		// <DataBase Host="127.0.0.1" Port="3306" Name="unreal_server" User="root" PW="1234" />
		tinyxml2::XMLElement* db_element = root_element->FirstChildElement("DataBase");

		s_DBHost	= nullptr != db_element->FindAttribute("Host")	? db_element->FindAttribute("Host")->Value() : "";
		s_DBPort	= nullptr != db_element->FindAttribute("Port")	? std::stoi(db_element->FindAttribute("Port")->Value()) : -1;
		s_DBSchema	= nullptr != db_element->FindAttribute("Name")	? db_element->FindAttribute("Name")->Value() : "";
		s_DBUser	= nullptr != db_element->FindAttribute("User")	? db_element->FindAttribute("User")->Value() : "";
		s_DBPw		= nullptr != db_element->FindAttribute("PW")	? db_element->FindAttribute("PW")->Value() : "";

		GameServerDebug::LogInfo("DB Config Info");

		GameServerDebug::LogInfo(std::string("DBHost : "	+ s_DBHost));
		GameServerDebug::LogInfo(std::string("DBPort : "	+ std::to_string(s_DBPort)));
		GameServerDebug::LogInfo(std::string("DBSchema : "	+ s_DBSchema));
		GameServerDebug::LogInfo(std::string("DBUser : "	+ s_DBUser));
		GameServerDebug::LogInfo(std::string("DBPw : "		+ s_DBPw));
	}

	if (false == CoreDataCheck())
	{
		GameServerDebug::LogErrorAssert("Server Or DB Data Is Not Valid");
		return false;
	}
	GameServerDebug::LogInfo("Server Data Check Done");

	DBQueue::Init();
	GameServerDebug::LogInfo("DBThread Initialize Done");

	NetQueue::Init();
	GameServerDebug::LogInfo("NetThread Initialize Done");

	ServerHelper::ServerStartup();
	GameServerDebug::LogInfo("Server Initialize Done");

	return true;
}

bool GameServerCore::CoreEnd()
{
	DBQueue::Destroy();
	NetQueue::Destroy();
	GameServerDebug::Destroy();

	return true;
}