#ifndef EPOLL_LOOP_H
#define EPOLL_LOOP_H
#include "Epoll.h"
#include "Channel.h"
class EpollLoop{

private:
    Epoll *ep_;//每个epoll对象对应一个epollloop事件循环
public:

    EpollLoop();
    ~EpollLoop();
    void run();
    Epoll * getEp();

};



#endif

