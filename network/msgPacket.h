#ifndef MSG_PACKET_H
#define MSG_PACKET_H

#include "network.h"

class msgPacket
{
public:
    msgPacket(uint64_t id,int length,const char* msg) : id(id),length(length),msg(msg) {}
    ~msgPacket() {}
    uint64_t getId() { return id; }
    void setId(uint64_t id) { this->id = id; }

    int getLength() { return length; }
    void setLength(int length) { this->length = length; }

    const char* getMsg() { return msg; }
    void setMsg(const char* msg) { this->msg = msg; }

private:
    uint64_t id;
    int length;
    const char* msg;
};

#endif // MSG_PACKET_H