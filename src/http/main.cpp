#include "Epoll.h"
#include "EventLoop.h"
#include "HttpServer.h"
#include "InetAddr.h"
#include "Socket.h"
#include "TcpServer.h"
#include "common.h"
#include <functional>
#include <signal.h>

HttpServer *serv;
// 相关信号处理函数
void sig_handler(int sig) {
  printf("接收到信号%d，开始退出程序\n", sig);
  serv->stop();
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    printf("Usage : epoll_server ip port\n");
    exit(-1);
  }
  // 获取ip与端口
  char *ip = argv[1];
  int port = atoi(argv[2]);
  signal(2, sig_handler);
  signal(15, sig_handler);
  serv = new HttpServer(ip, port, 4, 4);

  serv->start();
}