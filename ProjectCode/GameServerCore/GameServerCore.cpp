#include "PreCompile.h"
#include "GameServerCore.h"
#include <conio.h>
#include <GameServerBase/GameServerDebug.h>
#include <GameServerBase/GameServerDirectory.h>
#include <GameServerNet/ServerHelper.h>
#include <GameServerNet/UDPSession.h>
#include <GameServerNet/IPEndPoint.h>
#include "GameServerSectionManager.h"
#include "DBQueue.h"
#include "NetQueue.h"

int			GameServerCore::s_ServerPort;
int			GameServerCore::s_DBPort;
std::string	GameServerCore::s_DBHost;
std::string	GameServerCore::s_DBSchema;
std::string	GameServerCore::s_DBUser;
std::string	GameServerCore::s_DBPw;

TCPListener											GameServerCore::s_Listener;
std::function<void(std::shared_ptr<TCPSession>)>	GameServerCore::s_AcceptCallback;

IPEndPoint											GameServerCore::s_ServerEndPoint;
std::vector<std::shared_ptr<UDPSession>>			GameServerCore::s_AllUDPSession;
std::function<void(std::shared_ptr<UDPSession>, const std::vector<unsigned char>&, IPEndPoint&)> GameServerCore::s_UDPCallBack;

std::atomic<int> GameServerCore::s_UDPPortNumber;
std::atomic<int> GameServerCore::s_MaxUDPPortNumber;

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

void GameServerCore::SetAcceptCallBack(const std::function<void(std::shared_ptr<TCPSession>)>& callback)
{
	s_AcceptCallback = callback;
}

void GameServerCore::UDPInitialize(int udp_count, const std::function<void(std::shared_ptr<UDPSession>, const std::vector<unsigned char>&, IPEndPoint&)>& callback)
{
	s_UDPCallBack = callback;
	s_UDPPortNumber = 0;
	s_MaxUDPPortNumber = udp_count;

	for (int i = 0; i < udp_count; ++i)
	{
		IPEndPoint end_point = IPEndPoint(s_ServerEndPoint.GetAddress(), s_ServerEndPoint.GetPort() + i);

		std::shared_ptr<UDPSession> udp_session = std::make_shared<UDPSession>();
		udp_session->Initialize(end_point, s_UDPCallBack);
		if (false == udp_session->BindQueue(NetQueue::GetQueue()))
		{
			int a = 0;
		}

		udp_session->Recv();
		s_AllUDPSession.push_back(udp_session);
	}
}

std::shared_ptr<UDPSession> GameServerCore::GetUDPPort(int& udp_port_dest)
{
	if (s_MaxUDPPortNumber <= s_UDPPortNumber)
	{
		// udp용 port 카운트 초기화
		s_UDPPortNumber = 0;
	}

	const int udp_index = s_UDPPortNumber;
	udp_port_dest = s_ServerEndPoint.GetPort() + s_UDPPortNumber++;

	if (udp_index >= static_cast<int>(s_AllUDPSession.size()))
	{
		GameServerDebug::AssertDebugMsg("Invalid Access To UDP Session Vector");
		return nullptr;
	}

	return s_AllUDPSession[udp_index];
}

bool GameServerCore::CoreInit()
{
	GameServerDebug::Initialize();
	GameServerDebug::LogInfo("Server Initialize Start");

	GameServerDirectory directory;
	if (false == directory.IsExistFile("ServerConfig.xml"))
	{
		directory.MoveToParent("ProjectCode");
	}

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

		s_ServerEndPoint = IPEndPoint(IPAddress::Parse("127.0.0.1"), s_ServerPort);
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

bool GameServerCore::CoreRun()
{
	if (nullptr == s_AcceptCallback)
	{
		GameServerDebug::LogErrorAssert("s_AcceptCallback Is Nullptr");
		return false;
	}

	s_Listener.Initialize(IPEndPoint(IPAddress::Parse("127.0.0.1"), s_ServerPort), s_AcceptCallback);

	s_Listener.BindQueue(NetQueue::GetQueue());
	s_Listener.StartAccept(10);

	while (true)
	{
		if ('`' == _getch())
		{
			return true;
		}
	}
}

bool GameServerCore::CoreEnd()
{
	DBQueue::Destroy();
	NetQueue::Destroy();
	GameServerDebug::Destroy();
	GameServerSectionManager::Destroy();

	return true;
}