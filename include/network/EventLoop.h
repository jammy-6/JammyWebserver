#ifndef EPOLL_LOOP_H
#define EPOLL_LOOP_H

#include "Epoll.h"
#include "InetAddr.h"
#include "Socket.h"
#include <atomic>
#include <deque>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <sys/eventfd.h>
#include <sys/timerfd.h>

//前置声明
class Connection;
class Channel;
class Epoll;
// Connection类的共享指针
using spConnection = std::shared_ptr<Connection>;
class EventLoop {

private:
  // EventLoop类封装的Epoll，是1对1关系
  std::unique_ptr<Epoll> ep_;
  // EPOLL类调用loop函数时超时的回调函数
  std::function<void()> epollTimeOutCallback_;

  //保护任务队列的互斥量
  std::mutex mut_;
  //任务队列，其中添加的任务主要都是来自工作线程的Connection::append
  //即对Connection输出缓冲区进行填充
  std::deque<std::function<void()>> taskque_;
  //对Connection输出缓冲区进行填充的channel
  std::unique_ptr<Channel> connOutBuffChannel_;

  //用于保护多线程下cons_写入cons_的问题
  std::mutex timerMut_;
  //当前EventLoop类接受连接的客户
  std::map<Socket *, spConnection> cons_;

  //是否为主事件循环，即主Reactor
  bool isMainLoop_;
  //当前事件循环的开启状态
  std::atomic_bool status_;
  //定时器到期时间，到期时检查所有连接的connection是否超时
  time_t clock_;
  //连接超时时长
  time_t timeout_;
  // timerfd对应的Channel
  std::unique_ptr<Channel> timerChannel_;
  //定时器回调函数，如果当前是从事件循环，则主要用于删除不活跃连接
  //当存在超时连接时调用的回调函数
  std::function<void(Socket *)> connTimeoutCallBack_;

public:
  //构造函数参数分别为 是否主循环，定时器到期时间，连接超时时间
  EventLoop(bool isMainLoop, time_t clock, time_t timeout);
  //析构函数
  ~EventLoop();
  //开始事件循环,会调用Epoll::loop函数
  void run();
  //关闭事件循环,设置staus_为false
  void stop();
  //获取Epoll指针
  Epoll *getEp();
  //往任务队列中加入任务
  void addTask(std::function<void()>);
  // eventChannel读事件发生，从任务队列中取出任务进行处理
  void handleConnOutbufflMsg();
  //处理timerChannel_定时器到期事件
  void handleTimerChannelMsg();
  //往当前map中添加连接
  void appendConn(Socket *, spConnection);
  //往当前map中删除连接
  void removeConn(Socket *cliSocket);
  //设置Epoll::loop超时的回调函数
  void setEpollTimeOutCallback(std::function<void()> func);
  // map中的连接超时的回调函数
  void setConnTimeoutCallBack(std::function<void(Socket *)> func);
};

#endif
