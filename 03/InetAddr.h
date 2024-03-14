#ifndef INET_ADDR_H
#define INET_ADDR_H
#include "common.h"

//地址类
class InetAddr{
public:
    InetAddr(const char*ip,short port);
    InetAddr(sockaddr_in addr_);

    sockaddr *addr();
    const char *ip()const;
    int port()const;

private:
    sockaddr_in addr_;
};

#endif