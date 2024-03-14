#ifndef EPOLL_LOOP_H
#define EPOLL_LOOP_H
#include "Epoll.h"
#include <functional>

class Epoll;
class EpollLoop{

private:
    Epoll *ep_;//每个epoll对象对应一个epollloop事件循环
    std::function<void(EpollLoop*)> epollTimeOutCallback_; //EPOLL超时回调函数
public:
    void setEpollTimeOutCallback(std::function<void(EpollLoop*)> func);
    EpollLoop();
    ~EpollLoop();
    void run();
    Epoll * getEp();
};



#endif

