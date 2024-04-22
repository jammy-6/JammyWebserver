#include "EchoServer.h"
#include "Type.h"

EchoServer::EchoServer(std::string ip, uint16_t port, int threadnum,
                       int workThreadNum)
    : tcpServer_(ip, port, ECHO,threadnum), tp_(workThreadNum, "WORK"),
      workThreadNum_(threadnum) {

  tcpServer_.setOnNewConnectionCallback(
      std::bind(&EchoServer::handleNewConnection, this, std::placeholders::_1));
  tcpServer_.setOnConnCloseCallback(
      std::bind(&EchoServer::handleConnClose, this, std::placeholders::_1));
  tcpServer_.setOnConnErrorCallback(
      std::bind(&EchoServer::handleConnError, this, std::placeholders::_1));
  tcpServer_.setOnMessageCallback(std::bind(&EchoServer::handleOnMessage, this,
                                            std::placeholders::_1,
                                            std::placeholders::_2));
  tcpServer_.setOnMsgSendCompleteCallback(std::bind(
      &EchoServer::handleOnMessageSendComplete, this, std::placeholders::_1));
  tcpServer_.setOnConnTimeOutCallback(
      std::bind(&EchoServer::handleOnConnTimeOut, this, std::placeholders::_1));
}
EchoServer::~EchoServer() {}

void EchoServer::handleNewConnection(Socket *cliSocket) {

  printf("[SERVER] : Client Connect Success, Ip = %s, Port = %hu, ThreadId = "
         "%ld\n",
         cliSocket->getAddr()->ip(), cliSocket->getAddr()->port(),
         syscall(SYS_gettid));
}
void EchoServer::handleConnError(Socket *cliSocket) {
  printf("[SERVER] : Client Error %s:%d. Disconnect, ThreadId = %ld \n",
         cliSocket->getAddr()->ip(), cliSocket->getAddr()->port(),
         syscall(SYS_gettid));
}
void EchoServer::handleConnClose(Socket *cliSocket) {
  printf("[SERVER] : Client Close %s:%d. Disconnect, ThreadId = %ld \n",
         cliSocket->getAddr()->ip(), cliSocket->getAddr()->port(),
         syscall(SYS_gettid));
}
void EchoServer::handleOnMessage(spConnection con, std::string &data) {
  printf("[SERVER] : Recv : %s, ThreadId = %ld\n", data.data(),
         syscall(SYS_gettid));
  tp_.addTask(std::bind(&EchoServer::sendMessage, this, con, data));
}
void EchoServer::handleOnMessageSendComplete(spConnection con) {
  printf("[SERVER] : Connection %s:%d Send Complete, ThreadId = %ld\n",
         con->getCliSocket()->getAddr()->ip(),
         con->getCliSocket()->getAddr()->port(), syscall(SYS_gettid));
}
void EchoServer::handleOnEpollTimeOut(EventLoop *loop) {}
void EchoServer::handleOnConnTimeOut(Socket *cliSocket) {
  printf(
      "[SERVER] : Client Connect Timeout %s:%d. Disconnect, ThreadId = %ld \n",
      cliSocket->getAddr()->ip(), cliSocket->getAddr()->port(),
      syscall(SYS_gettid));
}
void EchoServer::start() {
  printf("[SERVER] EchoServer Start Successful\n");
  tcpServer_.start();
}

void EchoServer::stop() {
  printf("[SERVER] EchoServer Stop\n");
  tp_.stop();
  tcpServer_.stop();
}
// 用于处理客户端业务（工作线程）
void EchoServer::sendMessage(spConnection con, std::string &data) {
  con->getEventLoop()->addTask(std::bind(&Connection::writeOutBuff, con, data));
  // con->writeOutBuff(data);
}
