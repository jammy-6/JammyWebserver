#ifndef CHANNEL_H
#define CHANNEL_H


#include "common.h"
#include "Epoll.h"
#include "Socket.h"
#include "InetAddr.h"
class Epoll;

class Channel{
private:
    int fd_=-1;//channel类对应的fd，fd与channel是一对一关系
    Epoll *ep_;//channel类对应的Epoll指针， ep_与channel是一对多的关系
    bool isInEpoll_=false;//判断当前channel是否被监听
    uint32_t events_ =0 ;//当前channel被epoll类监听的事件
    uint32_t revents_ =0 ;//当前channel在epoll_wait中实际发生的事件
    bool isListen_ = false;//当前channel是否是listen channel
    char buff_[1024];//当前channel读取数据，发送数据所用
    bool valid_ = true;//当前信道是否有效，ep->removeChannel时标记
public:
    Channel(int fd,Epoll *ep,bool isListen=false);
    ~Channel();
    bool isValid();
    void setInEpoll(bool on);
    void setValid(bool on);
    void setet();
    void enableReading();
    bool inEpoll()const;
    void setRevents(uint32_t rev);
    uint32_t events()const;
    uint32_t revents()const;
    int fd()const;
    void handleEvent(Socket *servSock);
};



#endif