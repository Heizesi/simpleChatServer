add_library(network "")

target_sources(network
PRIVATE
    network.cpp
    connection.cpp
    luaInterfere.cpp
PUBLIC
    network.h
    luaInterfere.h
    connection.h
    msgPacket.h
)

target_include_directories(network
PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)

find_package(Lua REQUIRED)
target_include_directories(network PRIVATE ${LUA_INCLUDE_DIR})
target_link_libraries(network PRIVATE ${LUA_LIBRARIES})

find_path(LUABRIDGE3_INCLUDE_DIRS "luabridge3/LuaBridge/Array.h")
target_include_directories(network PRIVATE ${LUABRIDGE3_INCLUDE_DIRS})

find_package(libuv CONFIG REQUIRED)
target_link_libraries(network PRIVATE $<IF:$<TARGET_EXISTS:libuv::uv_a>,libuv::uv_a,libuv::uv>)