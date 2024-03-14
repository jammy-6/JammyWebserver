#ifndef EPOLL_LOOP_H
#define EPOLL_LOOP_H
#include "Epoll.h"
#include <functional>
#include <memory>
class Epoll;
class EpollLoop{

private:
    std::unique_ptr<Epoll> ep_;//每个epoll对象对应一个epollloop事件循环
    std::function<void()> epollTimeOutCallback_; //EPOLL超时回调函数
public:
    void setEpollTimeOutCallback(std::function<void()> func);
    EpollLoop();
    ~EpollLoop();
    void run();
    Epoll * getEp();
};



#endif

