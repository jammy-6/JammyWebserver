#ifndef ECHOSERVER_h
#define ECHOSERVER_h


#include "TcpServer.h"
#include "common.h"

class EchoServer{
private:
    TcpServer tcpServer_;

public:

    EchoServer(std::string ip,uint16_t port);
    ~EchoServer();

    void handleNewConnection(Socket *cliSocket);  
    void handleConnError(Socket *cliSocket);
    void handleConnClose(Socket *cliSocket);
    void handleOnMessage(Connection *con,std::string &data);
    void handleOnMessageSendComplete(Connection *con);
    void handleOnEpollTimeOut(EpollLoop* loop);
    void start();
};



#endif

