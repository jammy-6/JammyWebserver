#include "HttpServer.h"
#include "Buffer.h"
#include "Type.h"
#include "config.h"
#include "httprequest.h"
#include "httpresponse.h"
#include "sqlconnpool.h"
#include <string>
HttpServer::HttpServer(std::string ip, uint16_t port, int threadnum,
                       int workThreadNum)
    : tcpServer_(ip, port, HTTP, threadnum), tp_(workThreadNum, "WORK"),
      workThreadNum_(threadnum) {
  SqlConnPool::Instance()->Init("127.0.0.1", 3306, "jammy", "jammy",
                                "jammy_server_db");
  tcpServer_.setOnNewConnectionCallback(
      std::bind(&HttpServer::handleNewConnection, this, std::placeholders::_1));
  tcpServer_.setOnConnCloseCallback(
      std::bind(&HttpServer::handleConnClose, this, std::placeholders::_1));
  tcpServer_.setOnConnErrorCallback(
      std::bind(&HttpServer::handleConnError, this, std::placeholders::_1));
  tcpServer_.setOnMessageCallback(std::bind(&HttpServer::handleOnMessage, this,
                                            std::placeholders::_1,
                                            std::placeholders::_2));
  tcpServer_.setOnMsgSendCompleteCallback(std::bind(
      &HttpServer::handleOnMessageSendComplete, this, std::placeholders::_1));
  tcpServer_.setOnConnTimeOutCallback(
      std::bind(&HttpServer::handleOnConnTimeOut, this, std::placeholders::_1));
}
HttpServer::~HttpServer() {}

void HttpServer::handleNewConnection(Socket *cliSocket) {

  printf("[SERVER] : Client Connect Success, Ip = %s, Port = %hu, ThreadId = "
         "%ld\n",
         cliSocket->getAddr()->ip(), cliSocket->getAddr()->port(),
         syscall(SYS_gettid));
}
void HttpServer::handleConnError(Socket *cliSocket) {
  printf("[SERVER] : Client Error %s:%d. Disconnect, ThreadId = %ld \n",
         cliSocket->getAddr()->ip(), cliSocket->getAddr()->port(),
         syscall(SYS_gettid));
}
void HttpServer::handleConnClose(Socket *cliSocket) {
  printf("[SERVER] : Client Close %s:%d. Disconnect, ThreadId = %ld \n",
         cliSocket->getAddr()->ip(), cliSocket->getAddr()->port(),
         syscall(SYS_gettid));
}
void HttpServer::handleOnMessage(spConnection con, std::string &data) {
  printf("[SERVER] : Recvive Below : \n%s, ThreadId = %ld\n", data.data(),
         syscall(SYS_gettid));

  // writev(con->getCliSocket()->fd(), iov_, 2);
  tp_.addTask(std::bind(&HttpServer::sendMessage, this, con, data));
}
void HttpServer::handleOnMessageSendComplete(spConnection con) {
  printf("[SERVER] : Connection %s:%d Send Complete, ThreadId = %ld\n",
         con->getCliSocket()->getAddr()->ip(),
         con->getCliSocket()->getAddr()->port(), syscall(SYS_gettid));
}
void HttpServer::handleOnEpollTimeOut(EventLoop *loop) {}
void HttpServer::handleOnConnTimeOut(Socket *cliSocket) {
  printf(
      "[SERVER] : Client Connect Timeout %s:%d. Disconnect, ThreadId = %ld \n",
      cliSocket->getAddr()->ip(), cliSocket->getAddr()->port(),
      syscall(SYS_gettid));
}
void HttpServer::start() {
  printf("[SERVER] HttpServer Start Successful\n");
  tcpServer_.start();
}

void HttpServer::stop() {
  printf("[SERVER] HttpServer Stop\n");
  tp_.stop();
  tcpServer_.stop();
}
// 用于处理客户端业务（工作线程）
void HttpServer::sendMessage(spConnection con, std::string &data) {
  HttpRequest request_;
  request_.Init();
  Buffer buff;
  buff.Append(data);
  HttpResponse response_;
  if (data.size() <= 0) {
    return;
  } else if (request_.parse(buff)) {
    response_.Init(RESOURCE, request_.path(), request_.IsKeepAlive(), 200);
  } else {
    response_.Init(RESOURCE, request_.path(), false, 400);
  }
  Buffer writeBuff_;
  // 添加响应行和响应头
  response_.MakeResponse(writeBuff_);
  std::string writeBuffStr(writeBuff_.Peek(), writeBuff_.ReadableBytes());
  // 添加响应体
  writeBuffStr.append(response_.File(), response_.FileLen());
  con->getEventLoop()->addTask(
      std::bind(&Connection::writeOutBuff, con, writeBuffStr));
  // con->writeOutBuff(data);
}
