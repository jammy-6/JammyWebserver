#ifndef EPOLL_H
#define EPOLL_H
#include "Channel.h"
#include "common.h"
#include <memory>
#include <vector>
class Channel;

//封装的epoll类
class Epoll {

private:
  // epoll对应的fd，由epoll_create返回
  int epollfd_;
  //每一次epoll_wait时监听的事件数量
  static const int MAX_EVENTS = 100;
  //由epollwait监听的实际发生的事件
  epoll_event evs_[MAX_EVENTS];

public:
  //创建Epoll，size为内核事件表的大小
  Epoll(int size = 100);
  //析构函数，会调用close关闭epollfd_
  ~Epoll();
  //将传入的Channel向内核时间表进行注册会修改
  void updateChannel(Channel *ch);
  //删除监听的信道
  void removeChannel(Channel *ch);
  // loop函数会调用epoll_wait监听内核中实际发生的事件，并封装成Channel返回
  std::vector<Channel *> loop(int timeout = -1);
};

#endif