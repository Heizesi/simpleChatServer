#ifndef NETWORK_H
#define NETWORK_H

extern "C"
{
    #include <lua.h>
    #include <lauxlib.h>
    #include <lualib.h>
}

#include <uv.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <algorithm>
#include <atomic>
#include <luabridge3/LuaBridge/LuaBridge.h>
#include "luaInterfere.h"
#include "connection.h"
#include "msgPacket.h"

class connection;

class server
{
public:
    server(const char* ip,int port);
    ~server();
    void start();
    void close();
    void broadcast(std::string msg);
    void sendto(uint64_t id,std::string msg);
    void onNewConnection(uv_stream_t* server_handle,int status);
    void closeConnection(uint64_t id);
    uint64_t getConnectionsCount() { connections_count++;return connections_count.load();}

private:
    int port;
    const char* ip;
    std::atomic_uint64_t connections_count;
    uv_loop_t* uvloop;
    uv_tcp_t* server_handle;
    std::map<uint64_t,connection*> connections;
};
#endif // NETWORK_H