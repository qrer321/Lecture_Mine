#pragma once
#include <GameServerNet/TCPSession.h>
#include <GameServerNet/UDPSession.h>
#include <GameServerMessage/Dispatcher.h>

extern Dispatcher<TCPSession> g_tcp_dispatcher;
extern Dispatcher<UDPSession> g_udp_dispatcher;

void DispatcherRegistration();