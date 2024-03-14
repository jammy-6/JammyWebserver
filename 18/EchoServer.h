#ifndef ECHOSERVER_h
#define ECHOSERVER_h


#include "TcpServer.h"
#include "common.h"
#include "ThreadPool.h"
class EchoServer{
private:
    TcpServer tcpServer_;
    ThreadPool tp_;//工作线程池
    int workThreadNum_;
public:

    EchoServer(std::string ip,uint16_t port,int threadnum,int workThreadNum);
    ~EchoServer();

    void handleNewConnection(Socket *cliSocket);  
    void handleConnError(Socket *cliSocket);
    void handleConnClose(Socket *cliSocket);
    void handleOnMessage(Connection *con,std::string &data);
    void handleOnMessageSendComplete(Connection *con);
    void handleOnEpollTimeOut(EpollLoop* loop);
    void start();

    void onMessage(Connection *con,std::string &data);
};



#endif

