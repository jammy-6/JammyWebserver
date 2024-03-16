#ifndef INET_ADDR_H
#define INET_ADDR_H
#include "common.h"

//封装的地质类
class InetAddr{
public:
    InetAddr();//空的构造函数
    InetAddr(const char*ip,uint16_t port);//构造函数
    InetAddr(sockaddr_in addr_);//构造函数

    sockaddr *addr(); //返回sockaddr指针，用于accept或bind
    const char *ip()const;//返回ip地址的点分十进制形式
    u_int16_t port()const;//返回端口号

private:
    sockaddr_in addr_; //封装的地址
};

#endif