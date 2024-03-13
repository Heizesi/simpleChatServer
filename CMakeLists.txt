cmake_minimum_required(VERSION 3.20 FATAL_ERROR)
project(testserver)

SET(PROJECT_NAME "testserver")
SET(TARGET_NAME ${PROJECT_NAME})
SET(CMAKE_CXX_STANDARD 17)
SET(SOURCES "testserver.cpp")

add_executable(${TARGET_NAME} ${SOURCES})
add_executable(client client.cpp)

add_subdirectory(network)

target_link_libraries(${TARGET_NAME} PRIVATE network)

find_package(Lua REQUIRED)
target_include_directories(${TARGET_NAME} PRIVATE ${LUA_INCLUDE_DIR})
target_link_libraries(${TARGET_NAME} PRIVATE ${LUA_LIBRARIES})
target_include_directories(client PRIVATE ${LUA_INCLUDE_DIR})
target_link_libraries(client PRIVATE ${LUA_LIBRARIES})

find_path(LUABRIDGE3_INCLUDE_DIRS "luabridge3/LuaBridge/Array.h")
target_include_directories(${TARGET_NAME} PRIVATE ${LUABRIDGE3_INCLUDE_DIRS})
target_include_directories(client PRIVATE ${LUABRIDGE3_INCLUDE_DIRS})


find_package(libuv CONFIG REQUIRED)
target_link_libraries(${TARGET_NAME} PRIVATE $<IF:$<TARGET_EXISTS:libuv::uv_a>,libuv::uv_a,libuv::uv>)
target_link_libraries(client PRIVATE $<IF:$<TARGET_EXISTS:libuv::uv_a>,libuv::uv_a,libuv::uv>)