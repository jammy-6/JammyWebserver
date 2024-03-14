#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "EpollLoop.h"
#include <string>
#include "Acceptor.h"
class TcpServer{
private:

    EpollLoop eventLoops_;
    Acceptor *acceptor_;
public:

    TcpServer(std::string ip,uint16_t port);
    ~TcpServer();

    void start();


};

#endif