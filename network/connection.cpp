#include "connection.h"
#include <string>

connection::connection(uv_tcp_t* handle,uv_timer_t* sendTimer,server* serverPtr) : handle(handle),sendTimer(sendTimer),serverPtr(serverPtr)
{
    this->handle->data = this;
    this->write_buffer_offset = 0;
    this->read_buffer_offset = 0;
    this->req = (uv_write_t *)malloc(sizeof(uv_write_t));
    this->req->data = this;
    this->write_buf_t = (uv_buf_t*)malloc(sizeof(uv_buf_t));
    this->write_buf_t->base = write_buffer;
    this->sendTimer->data = this;
    this->timerStarted = false;
}

connection::~connection(){}

void connection::freeRes()
{
    free(this->handle);
    uv_timer_stop(this->sendTimer);
    free(this->sendTimer);
    free(this->write_buf_t);
    free(this->req);
    this->~connection();
    return ;
}

void doFreeRes(uv_handle_t* handle)
{
    connection* conn = (connection*)handle->data;
    conn->freeRes();
    return ;
}

void connection::clear()
{
    std::cout<<"connection "<<this->getId()<<" closed"<<std::endl;
    uv_close((uv_handle_t*)this->handle, doFreeRes);
    return ;
}

void connection::close()
{
    this->serverPtr->closeConnection(this->getId());
    return ;
}



void doStartTimer(uv_timer_t* timer)
{
    ((connection*)timer->data)->sendCachedPacket();
    return ;
}

void connection::startTimer()
{
    uv_timer_start(this->sendTimer, doStartTimer, 500, 0);
    this->timerStarted = true;
    return ;
}

void connection::stopTimer()
{
    uv_timer_stop(this->sendTimer);
    this->timerStarted = false;
    return ;
}

void doNewPacket(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf)
{
    ((connection*)stream->data)->onNewPacket(nread,buf);
    free(buf->base);
    return ;
}

void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
    buf->base = (char*) malloc(suggested_size);
    buf->len = suggested_size;
    return ;
}

void connection::getNewPacket()
{
    uv_read_start((uv_stream_t*)this->handle, alloc_buffer,doNewPacket);
    return ;
}

void connection::onNewPacket(ssize_t nread, const uv_buf_t *buf)
{
    if (nread > 0)
    {
        if (nread <= MAX_BUFFER_SIZE - this->read_buffer_offset)
        {
            memcpy(this->read_buffer + this->read_buffer_offset, buf->base, nread);
            this->read_buffer_offset += nread;
            if (this->read_buffer_offset >= 4)
            {
                int startPos = 0;
                int packet_size = *(int*)(this->read_buffer + startPos);
                while (startPos + packet_size + 4 <= this->read_buffer_offset)
                {
                    std::string msg(this->read_buffer + startPos + 4, packet_size);
                    msgPacket recvPacket(this->getId(),packet_size,msg.c_str());
                    std::cout << "Received packet from " << this->getId() << ": " << msg << std::endl;
                    lua_interfere::getInstance()->messageParser(recvPacket);
                    startPos += packet_size + 4;
                    packet_size = *(int*)(this->read_buffer + startPos);
                }
                if (startPos + packet_size + 4 > this->read_buffer_offset)
                {
                    memmove(this->read_buffer, this->read_buffer + startPos, this->read_buffer_offset - startPos);
                    this->read_buffer_offset -= startPos;
                }
                else
                {
                    this->read_buffer_offset = 0;
                }
            }
        }
        else
        {
            std::cout << "error: too much message received"<< std::endl;
            
        }
    }
    else if(nread < 0 && nread != UV_EOF)
    {
        std::cout << "error " << uv_err_name(nread) << std::endl;
        this->close();
    }
    else if(nread == UV_EOF)
    {
        std::cout << "connection " << this->getId() << "closed" << std::endl;
        this->close();
    }
    return ;
}

void connection::sendPacket(msgPacket sendPacket)
{
    if (sizeof(int) + sendPacket.getLength() <= MAX_BUFFER_SIZE - this->write_buffer_offset)
    {
        int len = strlen(sendPacket.getMsg());
        memcpy(this->write_buffer + this->write_buffer_offset, &len, sizeof(int));
        memcpy(this->write_buffer + this->write_buffer_offset + sizeof(int), sendPacket.getMsg(), len);
        this->write_buffer_offset += sizeof(int) + len;
        if (!this->timerStarted)
        {
            this->startTimer();
        }
    }
    else if (sizeof(int) + sendPacket.getLength() <= MAX_BUFFER_SIZE)
    {
        this->sendCachedPacket();
        int len = strlen(sendPacket.getMsg());
        memcpy(this->write_buffer + this->write_buffer_offset, &len, sizeof(int));
        memcpy(this->write_buffer + this->write_buffer_offset + sizeof(int), sendPacket.getMsg(), len);
        this->write_buffer_offset += sizeof(int) + len;
    }
    else
    {
        std::cout << "error: packet too large" << std::endl;
    }
    return ;
}

void onSendFinshed(uv_write_t* req, int status)
{
    std::cout << "send finished" << std::endl;
    ((connection*)req->data)->clearWriteBuffer();
    return ;
}

void connection::sendCachedPacket()
{
    if (this->write_buffer_offset > 0)
    {
        if (this->timerStarted)
        {
            this->stopTimer();
        }
        this->write_buf_t->len = this->write_buffer_offset;
        std::cout << "sending message " << this->write_buf_t->len << " bytes" << std::endl;
        uv_write(this->req,(uv_stream_t*)this->handle,this->write_buf_t,1,onSendFinshed);
    }
    return ;
}