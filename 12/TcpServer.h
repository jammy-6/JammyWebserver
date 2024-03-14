#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "EpollLoop.h"
#include <string>
#include "Acceptor.h"
#include "Connection.h"
class TcpServer{
private:

    EpollLoop eventLoops_;
    Acceptor *acceptor_;
public:

    TcpServer(std::string ip,uint16_t port);
    ~TcpServer();
    void newConnection(Socket *cliSocket);

    void start();


};

#endif