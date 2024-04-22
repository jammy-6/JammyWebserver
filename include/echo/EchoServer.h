#ifndef ECHOSERVER_h
#define ECHOSERVER_h

#include "EventLoop.h"
#include "TcpServer.h"
#include "ThreadPool.h"
#include "common.h"
#include <sys/syscall.h>
#include "Type.h"
class EchoServer {
private:
  //封装的TcpServer
  TcpServer tcpServer_;
  // 工作线程池
  ThreadPool tp_;
  //工作线程数量
  int workThreadNum_;

public:
  //构造函数
  EchoServer(std::string ip, uint16_t port, int threadnum, int workThreadNum);
  ~EchoServer();

  //往TcpServer中注册的回调函数
  void handleNewConnection(Socket *cliSocket);
  void handleConnError(Socket *cliSocket);
  void handleConnClose(Socket *cliSocket);
  void handleOnMessage(spConnection con, std::string &data);
  void handleOnMessageSendComplete(spConnection con);
  void handleOnEpollTimeOut(EventLoop *loop);
  void handleOnConnTimeOut(Socket *cliSocket);
  void sendMessage(spConnection con, std::string &data);
  // TcpServer关闭时的回调函数，停止EchoServer，关闭工作线程池
  void stop();
  //开启EchoServer
  void start();
};

#endif
