#ifndef EPOLL_H
#define EPOLL_H
#include "common.h"
#include <vector>

class Epoll{

public:
    Epoll(int size=100);
    void addfd(int fd,uint32_t op);
    std::vector<epoll_event>  loop(int timeout=-1);

private:
    static const int MAX_EVENTS = 100;
    int epollfd_;//
    epoll_event evs_[MAX_EVENTS]; //epollwait 需要监听的事件
};


#endif