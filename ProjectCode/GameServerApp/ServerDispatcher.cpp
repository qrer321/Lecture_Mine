#include "PreCompile.h"																													
#include "ServerDispatcher.h"																												
#include <GameServerBase/GameServerDebug.h>																									
																																			
#include "ThreadHandlerChatMessage.h"															
#include "ThreadHandlerLoginMessage.h"																	
																																			
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
																																			
	std::shared_ptr<MessageHandler> message_handler = std::make_shared<MessageHandler>(std::move(tcp_session), std::move(convert_message));	
	message_handler->Start();																												
}																																			
																																			
void DispatcherRegistration()																												
{																																																															
		g_dispatcher.AddHandler(static_cast<uint32_t>(MessageType::Chat),	
		[](std::shared_ptr<TCPSession> tcp_session, std::shared_ptr<GameServerMessage> message)					
		{																										
			return OnMessageProcess<ThreadHandlerChatMessage, ChatMessage>(std::move(tcp_session), std::move(message));			
		});																										
																												
		g_dispatcher.AddHandler(static_cast<uint32_t>(MessageType::Login),			
		[](std::shared_ptr<TCPSession> tcp_session, std::shared_ptr<GameServerMessage> message)					
		{																										
			return OnMessageProcess<ThreadHandlerLoginMessage, LoginMessage>(std::move(tcp_session), std::move(message));					
		});																										
}																																		     