#include "Epoll.h"

/*
class Epoll{

public:
    Epoll(int size=100);
    void addfd(int fd,u_int32_t op);
    std::vector<epoll_event>  loop(int timeout);

private:
    static const int MAX_EVENTS = 100;
    int epollfd;//
    epoll_event evs[MAX_EVENTS]; //epollwait 需要监听的事件
};*/

Epoll::Epoll(int size){
    epollfd_ = epoll_create(16);
    bzero(&evs_,sizeof(evs_));
}
void Epoll::addfd(int fd,uint32_t op){
        //设置监听事件
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = (EPOLLET|EPOLLIN);
    
    epoll_ctl(epollfd_,EPOLL_CTL_ADD,fd,&ev);
}
std::vector<epoll_event>  Epoll::loop(int timeout){
    std::vector<epoll_event> curevs;
    int nums = epoll_wait(epollfd_,evs_,MAX_EVENTS,timeout);
    if(nums==0){
        //超时事件
        printf("[SERVER] : Epoll Timeout!\n");
        return curevs;
    }else if(nums<0){//epoll失败
        printf("File : %s, Function : %s,Line : %d, Errno : %d\n",__FILE__,__FUNCTION__,__LINE__,errno);
        return curevs;
    }
    for(int i=0;i<nums;i++){
        curevs.push_back(evs_[i]);
    }
    return curevs;
}