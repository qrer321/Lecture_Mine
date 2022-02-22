#pragma once
#include <GameServerMessage/Dispatcher.h>

extern Dispatcher<TCPSession> g_dispatcher;
void DispatcherRegistration();