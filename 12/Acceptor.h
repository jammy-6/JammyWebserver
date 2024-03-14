#ifndef ACCEPTOR_H
#define ACCEPTOR_H
#include "Channel.h"
#include "EpollLoop.h"
#include "Socket.h"
#include <string>
#include <functional>
class Acceptor{

private:
    EpollLoop *eventsLoop_; //从外面传入的的EpollLoop
    Channel *acceptChannel_;//用于接受客户连接请求的channel
    Socket *servSocket_; //服务端socket地址
    std::function<void(Socket *)> newConnCallBack_;
public:
    Acceptor( std::string &ip,u_int16_t port,EpollLoop *eventsLoop);
    ~Acceptor();

    void setNewConnCallBack(std::function<void(Socket *)> func);
    void newConnection();
};



#endif