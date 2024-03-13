#ifndef LUA_INTERFRE_H
#define LUA_INTERFRE_H

#include "network.h"
#include "msgPacket.h"

class lua_interfere
{
public:
    void messageParser(msgPacket recvPacket);
    void start();
    void stop();
    static lua_interfere* getInstance()
    {
        static lua_interfere instance;
        return &instance;
    }
protected:
    lua_interfere();
    ~lua_interfere();
private:
    lua_State *L;
};

#endif // LUA_INTERFRE_H