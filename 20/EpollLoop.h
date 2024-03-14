#ifndef EPOLL_LOOP_H
#define EPOLL_LOOP_H
#include "Epoll.h"
#include <functional>
#include <memory>
#include <deque>
#include <mutex>
#include <sys/eventfd.h>
#include <memory>
#include "Channel.h"
#include "Socket.h"
#include "InetAddr.h"
class Channel;
class Epoll;
class EpollLoop{

private:
    std::unique_ptr<Epoll> ep_;//每个epoll对象对应一个epollloop事件循环
    std::function<void()> epollTimeOutCallback_; //EPOLL超时回调函数

    //任务队列，主要用于处理工作线程的对Connection类的输出缓冲区输出
    //将这类工作移交给IO线程处理
    std::mutex mut_;
    std::deque<std::function<void()>> taskque_;
    std::unique_ptr<Channel> eventChannel_;//对输出缓冲区的写Channel类

public:
    void setEpollTimeOutCallback(std::function<void()> func);
    EpollLoop();
    ~EpollLoop();
    void run();
    Epoll * getEp();
    void addTask(std::function<void()> );
    void handleEventChannelMsg();//eventChannel读事件发生，从任务队列中取出任务进行处理


};



#endif

