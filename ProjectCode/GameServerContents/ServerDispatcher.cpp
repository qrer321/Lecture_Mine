#include "PreCompile.h"																													
#include "ServerDispatcher.h"																												
#include <GameServerBase/GameServerDebug.h>																									
																																			
#include "ThreadHandlerChatMessage.h"															
#include "ThreadHandlerPlayerUpdateMessage.h"															
#include "ThreadHandlerLoginMessage.h"																	
#include "ThreadHandlerJoinMessage.h"																	
#include "ThreadHandlerCreateCharacterMessage.h"																	
#include "ThreadHandlerSelectCharacterMessage.h"																	
#include "ThreadHandlerClientToReadyMessage.h"																	
#include "ThreadHandlerUDPConnectResultMessage.h"																	
																																			
Dispatcher<TCPSession> g_tcp_dispatcher;																									
Dispatcher<UDPSession> g_udp_dispatcher;																									
																																			
template <typename MessageHandler, typename MessageType, typename SessionType = TCPSession>													
void OnMessageProcess(std::shared_ptr<SessionType> session, std::shared_ptr<GameServerMessage> message)										
{																																			
	std::shared_ptr<MessageType> convert_message = std::dynamic_pointer_cast<MessageType>(std::move(message));								
	if (nullptr == convert_message)																											
	{																																		
		GameServerDebug::LogError("MessageConvert Error");																				
		return;																																
	}																																		
																																			
	std::shared_ptr<MessageHandler> message_handler = std::make_shared<MessageHandler>();													
	message_handler->Init(std::move(session), std::move(convert_message));																
	message_handler->Start();																												
}																																			
																																			
void DispatcherRegistration()																												
{																																																																
		g_tcp_dispatcher.AddHandler(static_cast<uint32_t>(MessageType::Chat), std::bind(&OnMessageProcess<ThreadHandlerChatMessage, ChatMessage>, std::placeholders::_1, std::placeholders::_2));				
																													
		g_tcp_dispatcher.AddHandler(static_cast<uint32_t>(MessageType::PlayerUpdate), std::bind(&OnMessageProcess<ThreadHandlerPlayerUpdateMessage, PlayerUpdateMessage>, std::placeholders::_1, std::placeholders::_2));				
																													
		g_tcp_dispatcher.AddHandler(static_cast<uint32_t>(MessageType::Login), std::bind(&OnMessageProcess<ThreadHandlerLoginMessage, LoginMessage>, std::placeholders::_1, std::placeholders::_2));						
																													
		g_tcp_dispatcher.AddHandler(static_cast<uint32_t>(MessageType::Join), std::bind(&OnMessageProcess<ThreadHandlerJoinMessage, JoinMessage>, std::placeholders::_1, std::placeholders::_2));						
																													
		g_tcp_dispatcher.AddHandler(static_cast<uint32_t>(MessageType::CreateCharacter), std::bind(&OnMessageProcess<ThreadHandlerCreateCharacterMessage, CreateCharacterMessage>, std::placeholders::_1, std::placeholders::_2));						
																													
		g_tcp_dispatcher.AddHandler(static_cast<uint32_t>(MessageType::SelectCharacter), std::bind(&OnMessageProcess<ThreadHandlerSelectCharacterMessage, SelectCharacterMessage>, std::placeholders::_1, std::placeholders::_2));						
																													
		g_tcp_dispatcher.AddHandler(static_cast<uint32_t>(MessageType::ClientToReady), std::bind(&OnMessageProcess<ThreadHandlerClientToReadyMessage, ClientToReadyMessage>, std::placeholders::_1, std::placeholders::_2));						
																													
		g_tcp_dispatcher.AddHandler(static_cast<uint32_t>(MessageType::UDPConnectResult), std::bind(&OnMessageProcess<ThreadHandlerUDPConnectResultMessage, UDPConnectResultMessage>, std::placeholders::_1, std::placeholders::_2));						
}																														