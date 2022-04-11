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
																																			
Dispatcher<TCPSession> g_dispatcher;																										
																																			
template <typename MessageHandler, typename MessageType>																					
void OnMessageProcess(std::shared_ptr<TCPSession> tcp_session, std::shared_ptr<GameServerMessage> message)									
{																																			
	std::shared_ptr<MessageType> convert_message = std::dynamic_pointer_cast<MessageType>(std::move(message));								
	if (nullptr == convert_message)																											
	{																																		
		GameServerDebug::LogError("MessageConvert Error");																				
		return;																																
	}																																		
																																			
	std::shared_ptr<MessageHandler> message_handler = std::make_shared<MessageHandler>();													
	message_handler->Init(std::move(tcp_session), std::move(convert_message));																
	message_handler->Start();																												
}																																			
																																			
void DispatcherRegistration()																												
{																																																															
		g_dispatcher.AddHandler(static_cast<uint32_t>(MessageType::Chat),	
		[](std::shared_ptr<TCPSession> tcp_session, std::shared_ptr<GameServerMessage> message)					
		{																										
			return OnMessageProcess<ThreadHandlerChatMessage, ChatMessage>(std::move(tcp_session), std::move(message));			
		});																										
																												
		g_dispatcher.AddHandler(static_cast<uint32_t>(MessageType::PlayerUpdate),	
		[](std::shared_ptr<TCPSession> tcp_session, std::shared_ptr<GameServerMessage> message)					
		{																										
			return OnMessageProcess<ThreadHandlerPlayerUpdateMessage, PlayerUpdateMessage>(std::move(tcp_session), std::move(message));			
		});																										
																												
		g_dispatcher.AddHandler(static_cast<uint32_t>(MessageType::Login),			
		[](std::shared_ptr<TCPSession> tcp_session, std::shared_ptr<GameServerMessage> message)					
		{																										
			return OnMessageProcess<ThreadHandlerLoginMessage, LoginMessage>(std::move(tcp_session), std::move(message));					
		});																										
																												
		g_dispatcher.AddHandler(static_cast<uint32_t>(MessageType::Join),			
		[](std::shared_ptr<TCPSession> tcp_session, std::shared_ptr<GameServerMessage> message)					
		{																										
			return OnMessageProcess<ThreadHandlerJoinMessage, JoinMessage>(std::move(tcp_session), std::move(message));					
		});																										
																												
		g_dispatcher.AddHandler(static_cast<uint32_t>(MessageType::CreateCharacter),			
		[](std::shared_ptr<TCPSession> tcp_session, std::shared_ptr<GameServerMessage> message)					
		{																										
			return OnMessageProcess<ThreadHandlerCreateCharacterMessage, CreateCharacterMessage>(std::move(tcp_session), std::move(message));					
		});																										
																												
		g_dispatcher.AddHandler(static_cast<uint32_t>(MessageType::SelectCharacter),			
		[](std::shared_ptr<TCPSession> tcp_session, std::shared_ptr<GameServerMessage> message)					
		{																										
			return OnMessageProcess<ThreadHandlerSelectCharacterMessage, SelectCharacterMessage>(std::move(tcp_session), std::move(message));					
		});																										
																												
		g_dispatcher.AddHandler(static_cast<uint32_t>(MessageType::ClientToReady),			
		[](std::shared_ptr<TCPSession> tcp_session, std::shared_ptr<GameServerMessage> message)					
		{																										
			return OnMessageProcess<ThreadHandlerClientToReadyMessage, ClientToReadyMessage>(std::move(tcp_session), std::move(message));					
		});																										
}																																		     