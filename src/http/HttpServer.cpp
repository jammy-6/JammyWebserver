#include "HttpServer.h"
#include "Buffer.h"
#include "Type.h"
#include "httprequest.h"
#include "httpresponse.h"
#include "sqlconnpool.h"
HttpServer::HttpServer(std::string ip, uint16_t port, int threadnum,
                       int workThreadNum)
    : tcpServer_(ip, port, HTTP,threadnum), tp_(workThreadNum, "WORK"),
      workThreadNum_(threadnum) {
    SqlConnPool::Instance()->Init("127.0.0.1",3306,"jammy","jammy","jammy_server_db");
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
  printf("[SERVER] : Recv : %s, ThreadId = %ld\n", data.data(),
         syscall(SYS_gettid));
    HttpRequest request_;
    request_.Init();
    Buffer buff;
    buff.Append(data);
    HttpResponse response_;
    if(data.size() <= 0) {
        return;
    }
    else if(request_.parse(buff)) {
        LOG_DEBUG("%s", request_.path().c_str());
        response_.Init("/home/jammy/code/jammy_webserver/resources", request_.path(), request_.IsKeepAlive(), 200);
    } else {
        response_.Init("/home/jammy/code/jammy_webserver/resources", request_.path(), false, 400);
    }
    Buffer writeBuff_;
    response_.MakeResponse(writeBuff_);
    struct iovec iov_[2];
    int iovCnt_;
    /* 响应头 */
    iov_[0].iov_base = const_cast<char*>(writeBuff_.Peek());
    iov_[0].iov_len = writeBuff_.ReadableBytes();
    iovCnt_ = 1;

    /* 文件 */
    if(response_.FileLen() > 0  && response_.File()) {
        iov_[1].iov_base = response_.File();
        iov_[1].iov_len = response_.FileLen();
        iovCnt_ = 2;
    }
    writev(con->getCliSocket()->fd(), iov_, 2);
  // tp_.addTask(std::bind(&HttpServer::sendMessage, this, con, data));
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
  con->getEventLoop()->addTask(std::bind(&Connection::writeOutBuff, con, data));
  // con->writeOutBuff(data);
}
