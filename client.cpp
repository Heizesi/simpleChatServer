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
#include <windows.h>

using namespace std;

void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
    buf->base = (char*) malloc(suggested_size);
    buf->len = suggested_size;
    return ;
}

void onRead(uv_stream_t *client, ssize_t nread, const uv_buf_t *sbuf)
{
    if (nread > 0)
    {
        cout<<string(sbuf->base+1,nread)<<endl;
    }
    return ;
}
uv_buf_t* buf = (uv_buf_t*)malloc(sizeof(uv_buf_t));
uv_write_t* write_req = (uv_write_t*)malloc(sizeof(uv_write_t));
uv_buf_t* readbuf = (uv_buf_t*)malloc(sizeof(uv_buf_t));
uv_stream_t* chatserver;
uv_tcp_t server;
struct sockaddr_in addr;
uv_connect_t conn_req;
char writebuffer[10240];

void onReadCmd(uv_stream_t *client, ssize_t nread, const uv_buf_t *sbuf)
{
    string cmdstr = string(sbuf->base, nread-1);
    buf->base = writebuffer;
    int size = cmdstr.size();
    memcpy(buf->base, &size, sizeof(int));
    memcpy(buf->base+sizeof(int), cmdstr.c_str(), size);
    buf->len = size+sizeof(int);
    uv_write(write_req,chatserver, buf, 1, NULL);
    return ;
}
// int cnt = 0;
// void onRepeat(uv_write_t *req, int status);

// void onReadCmd(uv_stream_t *client, ssize_t nread, const uv_buf_t *sbuf)
// {
//     string cmdstr = "this is a test message that long enough to be split into multiple packets----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------end.";
//     buf->base = writebuffer;
//     int size = cmdstr.size();
//     memcpy(buf->base, &size, sizeof(int));
//     memcpy(buf->base+sizeof(int), cmdstr.c_str(), size);
//     buf->len = size+sizeof(int);
//     uv_write(write_req,chatserver, buf, 1, onRepeat);
//     return ;
// }
// void onRepeat(uv_write_t *req, int status)
// {
//     if (*(int*)req->data < 1000 && status == 0)
//     {
//         Sleep(10);
//         uv_write(write_req,chatserver, buf, 1, onRepeat);
//         (*(int*)req->data)++;
//     }
//     return ;
// }

void onConnect(uv_connect_t *req, int status)
{
    if (status < 0)
    {
        uv_tcp_connect(&conn_req, &server, (const struct sockaddr*)&addr, onConnect);
        return ;
    }
    chatserver = (uv_stream_t*)req->handle;
    cout<<"start read cmd"<<endl;
    uv_read_start((uv_stream_t*)req->handle, alloc_buffer, onRead);
    return ;
}


int main()
{
    // write_req->data = &cnt;
    uv_loop_t *loop = uv_default_loop();
    uv_tcp_init(loop, &server);
    uv_ip4_addr("127.0.0.1", 1299, &addr);
    uv_tcp_connect(&conn_req, &server, (const struct sockaddr*)&addr, onConnect);
    uv_tty_t tty;
    uv_tty_init(uv_default_loop(), &tty, 0, 1);
    uv_stream_t *stream = (uv_stream_t *)&tty;
    uv_read_start(stream, alloc_buffer, onReadCmd);
    uv_run(loop, UV_RUN_DEFAULT);
    return 0;
}