#ifndef EPOLL_H
#define EPOLL_H
#include "common.h"
#include <vector>
#include "Channel.h"

class Channel;

class Epoll{

public:
    Epoll(int size=100);
    ~Epoll();
    void updateChannel(Channel *ch);
    std::vector<Channel *>  loop(int timeout=-1);
    void removeChannel(Channel *ch);
private:
    static const int MAX_EVENTS = 100;
    int epollfd_;//epoll对应的fd
    epoll_event evs_[MAX_EVENTS]; //epollwait 需要监听的事件
};


#endif