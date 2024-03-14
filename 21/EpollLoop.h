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
#include <sys/timerfd.h>
#include "Connection.h"
#include <map>

class Connection;
using spConnection=std::shared_ptr<Connection>;
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

    std::mutex timerMut_;//用于保护多线程下cons_写入的问题
    std::map<Socket *,spConnection> cons_;//当前EpollLoop类接受连接的客户

    bool isMainLoop_;//是否为主事件循环
    time_t clock_;//定时器到期时间
    time_t timeout_;//连接多久算超时
    std::unique_ptr<Channel> timerChannel_;//定时器回调函数，如果当前是从事件循环，则主要用于删除不活跃连接
    std::function<void(Socket *)> connTimeoutCallBack;//当存在超时连接时调用的回调函数

public:
    void setEpollTimeOutCallback(std::function<void()> func);
    EpollLoop(bool isMainLoop,time_t clock,time_t timeout);
    ~EpollLoop();
    void run();
    Epoll * getEp();
    void addTask(std::function<void()> );
    void handleEventChannelMsg();//eventChannel读事件发生，从任务队列中取出任务进行处理
    void handleTimerChannelMsg();//处理timerChannel_定时器到期事件

    void appendConn(Socket *,spConnection);//往当前map中添加连接
    void removeConn(Socket * cliSocket);
    void setConnTimeoutCallBack(std::function<void(Socket *)> func);
};



#endif

