#ifndef TCPSERVER_H
#define TCPSERVER_H
#include "Type.h"
#include "Acceptor.h"
#include "Connection.h"
#include "EventLoop.h"
#include "ThreadPool.h"
#include <map>
#include <memory>
#include <signal.h>
#include <string>
class TcpServer {
private:


  //TCPserver的类型，HTTP或ECHO
  TYPE type_;
  //主事件循环，用于接受连接
  std::unique_ptr<EventLoop> mainLoops_;
  //接受客户端请求
  std::unique_ptr<Acceptor> acceptor_;
  //保护cons_，主事件循环写入连接，从事件循环回调删除cons_操作
  std::mutex consMut_;
  //所有连接的客户端
  std::map<Socket *, spConnection> cons_;

  //线程数量，用于从事件，负责客户通信
  int threadnum_;
  //线程池，用与运行从事件循环
  std::unique_ptr<ThreadPool> thp_;
  //从事件循环
  std::vector<std::unique_ptr<EventLoop>> subLoops_;

  //连接创建时的回调函数，由上层应用如EchoServer进行注册
  std::function<void(Socket *)> onNewConnectionCallback_;
  //连接发生错误时的回调函数，由上层应用如EchoServer进行注册
  std::function<void(Socket *)> onConnErrorCallback_;
  //连接关闭时的回调函数，由上层应用如EchoServer进行注册
  std::function<void(Socket *)> onConnCloseCallback_;
  //连接有消息时的回调函数，由上层应用如EchoServer进行注册
  std::function<void(spConnection, std::string &)> onMessageCallback_;
  //连接数据发送完毕时的回调函数，由上层应用如EchoServer进行注册
  std::function<void(spConnection)> onMsgSendCompleteCallback_;
  // Epoll::loop超时的回调函数，由上层应用如EchoServer进行注册
  std::function<void(EventLoop *)> onEpollTimeOutCallback_;
  //连接超时的回调函数，由上层应用如EchoServer进行注册
  std::function<void(Socket *)> onConnTimeOutCallback_;
  // TcpServer停用时调用的回调函数
  std::function<void()> onServerStopCallback_;

public:
  TcpServer(std::string ip, uint16_t port,TYPE type,int threadnum = 3);
  ~TcpServer();
  // Acceptor类接受连接之后调用的回调函数
  void newConnection(Socket *cliSocket);
  // Connection类发生连接错误时调用的回调函数
  void connError(Socket *cliSocket);
  // Connection类关闭连接时调用的回调函数
  void connClose(Socket *cliSocket);
  // Connection类接受数据时调用的回调函数
  void onMessage(spConnection con, std::string &data);
  // Connection发送数据完毕时调用的回调函数
  void onMessageSendComplete(spConnection);
  // EventLoop类超时调用的回调函数
  void onEpollTimeOut(EventLoop *loop);

  //启动，调用EventLoop::run开启事件循环
  void start();
  //关闭服务器，删除所有连接并停止主事件循环和从事件循环
  void stop();
  // 往EventLoop::connTimeoutCallBack_注册的回调函数，用于连接超时
  void onConnTimeout(Socket *cliSocket);
  //处理2,15信号，让程序合理退出
  void sig_handler(int sig);
  //设置回调函数相关
  void setOnNewConnectionCallback(std::function<void(Socket *)> fun);
  void setOnConnErrorCallback(std::function<void(Socket *)> fun);
  void setOnConnCloseCallback(std::function<void(Socket *)> fun);
  void
  setOnMessageCallback(std::function<void(spConnection, std::string &)> fun);
  void setOnMsgSendCompleteCallback(std::function<void(spConnection)> fun);
  void setOnEpollTimeOutCallback(std::function<void(EventLoop *)> fun);
  void setOnConnTimeOutCallback(std::function<void(Socket *)> fun);
  void setOnServerStopCallback(std::function<void()> fun);
};

#endif