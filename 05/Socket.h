#ifndef SOCKET_H
#define SOCKET_H


#include "common.h"
#include "InetAddr.h"
//创建一个非阻塞的socket
int createnonblocking();
int setnonblocking(int fd);
class Socket
{
private:
    int fd_;//封装的socket
    InetAddr addr_;//socket所对应的地址
public:
    Socket();
    Socket(int fd,InetAddr addr); //构造函数
    ~Socket();
    int fd()const ;
    void setAddr(InetAddr addr);

    void bind();
    void listen(int blog=128);
    int accept(InetAddr &addr);
    void setReuseAddr(bool on);
    void setTCPNODELAY(bool on);
    void setKEEPALIVE(bool on);
    void setREUSEPORT(bool on);


};


#endif