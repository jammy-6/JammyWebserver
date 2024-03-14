#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "EpollLoop.h"
#include <string>
#include "Acceptor.h"
#include "Connection.h"
#include <map>
#include <string>
class TcpServer{
private:

    EpollLoop eventLoops_; //一个TcpServer对应一个事件循环
    Acceptor *acceptor_;//接受客户端请求
    std::map<Socket *,Connection*> cons_;//所有连接的客户端
public:

    TcpServer(std::string ip,uint16_t port);
    ~TcpServer();
    void newConnection(Socket *cliSocket);  
    void connError(Socket *cliSocket);
    void connClose(Socket *cliSocket);
    void onMessage(Connection *con,std::string &data);
    void onMessageSendComplete(Connection *con);
    void onEpollTimeOut(EpollLoop* loop);
    void start();


};

#endif