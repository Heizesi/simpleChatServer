#include "network.h"

server::server(const char* ip,int port) : ip(ip),port(port) {
    std::cout << "server created" << std::endl;
}

server::~server(){
    this->close();
}

void server::close()
{
    for (auto [name,conn]:this->connections)
    {
        conn->clear();
    }
    this->connections.clear();
    uv_close((uv_handle_t*)this->server_handle,[](uv_handle_t* handle){free(handle);});
    uv_loop_close(this->uvloop);
    free(this->uvloop);
    return ;
}

void doNewConnection(uv_stream_t* handle, int status) {
    ((server*)handle->data)->onNewConnection(handle,status);
    return ;
}

void server::start()
{
    this->connections_count.store((uint64_t)0);
    this->uvloop = (uv_loop_t*)malloc(sizeof(uv_loop_t));
    this->server_handle = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
    uv_loop_init(this->uvloop);
    this->server_handle->data = this;
    uv_tcp_init(this->uvloop, this->server_handle);
    struct sockaddr_in addr;
    uv_ip4_addr(this->ip, this->port, &addr);
    int error = uv_tcp_bind(this->server_handle, (const struct sockaddr*)&addr, 0);
    if (error)
    {
        std::cout<<"bind port error: "<<error<<std::endl;
    }
    error = uv_listen((uv_stream_t*)this->server_handle,128,doNewConnection);
    if (error)
    {
        std::cout<<"listen error: "<<error<<std::endl;
    }
    std::cout<<"server begin to listen"<<std::endl;
    error = uv_run(this->uvloop,UV_RUN_DEFAULT);
    if (error)
    {
        std::cout<<"run loop error: "<<error<<std::endl;
    }
    return ;
}

void server::onNewConnection(uv_stream_t* handle, int status){
    if (status < 0) {
		std::cout<<"connection status error: "<<status<<std::endl;
		return;
	}
	uv_tcp_t* newStream = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
	uv_tcp_init(this->uvloop, newStream);
    int error;
	error = uv_accept(handle, (uv_stream_t*)newStream);
	if (error == 0)
    {
        std::cout<<"a new connection accepted"<<std::endl;
        uv_timer_t* timer = (uv_timer_t*)malloc(sizeof(uv_timer_t));
        uv_timer_init(this->uvloop,timer);
        connection* newConn = new connection(newStream,timer,this);
        newConn->setId(this->getConnectionsCount());
        this->connections[newConn->getId()] = newConn;
        std::string welcome = "welcome to server, please login";
        newConn->sendPacket(msgPacket(newConn->getId(),welcome.length(),welcome.c_str()));
        newConn->getNewPacket();
	}
    else
    {
        uv_close((uv_handle_t*)newStream,[](uv_handle_t* handle){
            free(handle);
        });
        std::cout<<"accept connection error: "<<uv_strerror(error)<<std::endl;
    }
    return ;
}

void server::closeConnection(uint64_t id)
{
    if (this->connections.find(id) != this->connections.end())
    {
        this->connections[id]->clear();
        delete this->connections[id];
        this->connections.erase(id);
    }
    return ;
}

void server::sendto(uint64_t id,std::string msg)
{
    if (this->connections.count(id) != 0)
    {
        msgPacket pkt(id,msg.size(),msg.c_str());
        this->connections[id]->sendPacket(pkt);
    }
    return ;
}

void server::broadcast(std::string msg){
    for (auto [id,conn]:this->connections)
    {
        msgPacket pkt(id,msg.size(),msg.c_str());
        conn->sendPacket(pkt);
    }
    return ;
}