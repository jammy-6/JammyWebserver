#ifndef INET_ADDR_H
#define INET_ADDR_H
#include "common.h"

//封装的地址类
class InetAddr {
public:
  //空的构造函数
  InetAddr();
  //构造函数
  InetAddr(const char *ip, uint16_t port);
  //构造函数
  InetAddr(sockaddr_in addr_);

  //返回sockaddr指针，用于accept或bind
  sockaddr *addr();
  //返回ip地址的点分十进制形式
  const char *ip() const;
  //返回端口号
  u_int16_t port() const;

private:
  //封装的地址
  sockaddr_in addr_;
};

#endif