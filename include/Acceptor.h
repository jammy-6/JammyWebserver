#ifndef ACCEPTOR_H
#define ACCEPTOR_H
#include "Channel.h"
#include "EpollLoop.h"
#include "Socket.h"
#include <functional>
#include <memory>
#include <string>
class Acceptor {

private:
  //从外面传入的的EpollLoop
  EpollLoop *eventsLoop_;

  //服务端socket地址
  std::unique_ptr<Socket> servSocket_;
  //用于接受客户连接请求的channel
  std::unique_ptr<Channel> acceptChannel_;

  //接受新连接时的回调函数，由TcpServer注册
  std::function<void(Socket *)> newConnCallBack_;

public:
  //构造函数
  Acceptor(std::string &ip, u_int16_t port, EpollLoop *eventsLoop);
  //析构函数
  ~Acceptor();

  //设置回调函数
  void setNewConnCallBack(std::function<void(Socket *)> func);

  //当前acceptChannel_可读时调用的回调函数，调用Channel::setReadcallback注册
  void onNewConnection();
};

#endif