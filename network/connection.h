#ifndef CONNECTION_H
#define CONNECTION_H

#include "network.h"
#include "msgPacket.h"

#define MAX_BUFFER_SIZE 65535

class server;

class connection
{
public:
    connection(uv_tcp_t* handle,uv_timer_t* sendTimer,server* serverPtr);
    ~connection();
    void getNewPacket();
    void sendPacket(msgPacket sendPacket);
    void sendCachedPacket();
    void onNewPacket(ssize_t nread, const uv_buf_t *buf);
    uint64_t getId() { return id; }
    void setId(uint64_t id) { this->id = id; }
    void clearWriteBuffer() { write_buffer_offset = 0;}
    void startTimer();
    void stopTimer();
    void clear();
    void freeRes();
private:
    void close();
    int read_buffer_offset;
    int write_buffer_offset;
    bool timerStarted;
    char read_buffer[MAX_BUFFER_SIZE];
    char write_buffer[MAX_BUFFER_SIZE];
    uint64_t id;
    uv_timer_t* sendTimer;
    uv_buf_t* write_buf_t;
    uv_tcp_t* handle;
    uv_write_t *req;
    server* serverPtr;
};

#endif // CONNECTION_H