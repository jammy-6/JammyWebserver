#ifndef SOCKET_H
#define SOCKET_H

#include "InetAddr.h"
#include "common.h"

//创建一个非阻塞的socket
int createnonblocking();
//将传入的fd设置为非阻塞
int setnonblocking(int fd);

class Socket {
private:
  //封装的socket
  int fd_;
  // socket所对应的地址
  InetAddr addr_;

public:
  //空构造函数
  Socket();
  //构造函数
  Socket(int fd, InetAddr addr);
  //析构函数，close对应的fd
  ~Socket();
  //返回fd
  int fd() const;
  //设置地址
  void setAddr(InetAddr addr);
  //返回当前地址的指针
  InetAddr *getAddr();
  //将socket与地址进行绑定
  void bind();
  //对socket进行监听
  void listen(int blog = 128);
  //服务端socket同意客户端连接
  int accept(InetAddr &addr);
  //设置fd的相关属性
  void setReuseAddr(bool on);
  void setTCPNODELAY(bool on);
  void setKEEPALIVE(bool on);
  void setREUSEPORT(bool on);
};

#endif