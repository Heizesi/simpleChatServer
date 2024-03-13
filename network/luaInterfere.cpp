#include "luaInterfere.h"

lua_interfere::lua_interfere()
{
    this->L = luaL_newstate();
    luabridge::enableExceptions(L);
    luaL_openlibs(L);
    luabridge::getGlobalNamespace(L)
    .beginClass<server>("server")
    .addConstructor<void(const char*,int)>()
    .addFunction("start",&server::start)
    .addFunction("close",&server::close)
    .addFunction("broadcast",&server::broadcast)
    .addFunction("sendto",&server::sendto)
    .addFunction("closeConnection",&server::closeConnection)
    .endClass();
    int error = luaL_dofile(L,"start.lua");
    if (error == LUA_ERRFILE)
    {
        std::cout<<"can't load start.lua"<<std::endl;
    }
    error = luaL_dofile(L,"parser.lua");
    if (error == LUA_ERRFILE)
    {
        std::cout<<"can't load parser.lua"<<std::endl;
    }
}

lua_interfere::~lua_interfere()
{
    lua_close(L);
}

void lua_interfere::start()
{
    std::cout<<"starting lua vm"<<std::endl;
    luabridge::LuaRef starter=luabridge::getGlobal(L,"start");
    if (!starter.isFunction())
    {
        std::cout<<"can't find function start"<<std::endl;
    }
    luabridge::LuaResult res = starter();
    if (!res)
    {
        std::cerr<<res.errorMessage()<<std::endl;
    }
    return ;
}

void lua_interfere::stop()
{
    luabridge::LuaRef close=luabridge::getGlobal(L,"close");
    if (!close.isFunction())
    {
        std::cout<<"can't find function close"<<std::endl;
    }
    luabridge::LuaResult res = close();
    if (!res)
    {
        std::cerr<<res.errorMessage()<<std::endl;
    }
    return ;
}

void lua_interfere::messageParser(msgPacket recvPacket)
{
    luabridge::LuaRef parse=luabridge::getGlobal(L,"parse");
    if (!parse.isFunction())
    {
        std::cout<<"can't find function parse"<<std::endl;
    }
    luabridge::LuaResult res = parse(recvPacket.getId(),recvPacket.getMsg());
    if (!res)
    {
        std::cerr<<res.errorMessage()<<std::endl;
    }
    return ;
}