#ifndef CONNECTION_H
#define CONNECTION_H
#include "Buffer.h"
#include "Channel.h"
#include "EpollLoop.h"
#include "Socket.h"
#include "TimeStamp.h"
#include "common.h"
#include <atomic>
#include <functional>
#include <iostream>
#include <memory>
#include <string>

class Channel;
class EpollLoop;
class Connection;
// Connection类的共享指针
using spConnection = std::shared_ptr<Connection>;

// connection类，与每个客户端连接时都会由Acceptor回调TcpServer::newConnection创建
class Connection : public std::enable_shared_from_this<Connection> {

private:
  //从外面传入的的EpollLoop，Connection与EpollLoop是多对一关系
  EpollLoop *eventsLoop_;
  //封装的与客户进行通信的channel
  std::unique_ptr<Channel> connectionChannel_;
  //客户端socket
  std::unique_ptr<Socket> cliSocket_;
  //当前连接是否关闭的标志
  std::atomic_bool isClose_;

  //当前Connection发生读事件时的辅助缓存区
  //由于对写事件的处理存在并发，不能用于辅助写事件
  char buff_[1024];
  //当前连接输入缓冲区
  Buffer inBuff_;
  //当前连接输出缓冲区
  Buffer outBuff_;
  //记录当前Connection最后一次更新的时间戳
  TimeStamp timeStamp_;

  //当前连接发生错误时的回调函数，由注册TcpServer类注册
  std::function<void(Socket *)> errorCallBack_;
  //当前连接被关闭时的回调函数，由注册TcpServer类注册
  std::function<void(Socket *)> closeCallBack_;

  //当前连接可读时的回调函数，由注册TcpServer类注册
  std::function<void(spConnection, std::string &)> readCallBack_;
  //当前发送数据完毕时的回调函数，由注册TcpServer类注册
  std::function<void(spConnection)> sendCompleteCallBack_;

public:
  //构造函数
  Connection(EpollLoop *eventsLoop, Socket *cliSocket);
  //析构函数
  ~Connection();
  //获取当前连接的socket
  Socket *getCliSocket();

  // TCPSERVER 类往当前输出缓冲区中添加数据
  void writeOutBuff(std::string &data);
  //当前连接有消息可读时的处理函数，往Channel注册的readcallback_
  void onMessageIn();
  //当前连接outBuff_有数据可写的处理函数，往Channel注册的writecallback_
  void onMessageOut();
  //当前连接关闭时的处理函数，往Channel注册的closecallback_
  void onClose();

  //获取当前连接对应的事件循环
  EpollLoop *getEventLoop();
  //关闭当前连接，调用setIsClose和Channel::disableAll
  void removeChannel();
  //获取当前连接关闭状态
  bool getIsClose();
  //设置当前连接关闭状态
  void setIsClose(bool isClose);
  //判断连接是否超时
  bool isTimeOut(TimeStamp t, time_t timeout);

  //设置回调函数
  void setErrorCallBack(std::function<void(Socket *)> callback);
  void setCloseCallBack(std::function<void(Socket *)> callback);
  void
  setMessageCallBack(std::function<void(spConnection, std::string &)> callback);
  void setSendCompleteCallBack(std::function<void(spConnection)> callback);
};

#endif