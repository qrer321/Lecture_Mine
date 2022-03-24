#pragma once
#include <GameServerNet/TCPSession.h>
#include <GameServerMessage/Dispatcher.h>

extern Dispatcher<TCPSession> g_dispatcher;
void DispatcherRegistration();