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

Epoll::~Epoll() { close(epollfd_); }
Epoll::Epoll(int size) {
  epollfd_ = epoll_create(16);
  bzero(&evs_, sizeof(evs_));
}
void Epoll::updateChannel(Channel *ch) {
  //设置监听事件
  struct epoll_event ev;
  ev.data.ptr = ch;
  ev.events = ch->events();
  if (ch->isInEpoll()) {
    epoll_ctl(epollfd_, EPOLL_CTL_MOD, ch->fd(), &ev);
  } else {
    epoll_ctl(epollfd_, EPOLL_CTL_ADD, ch->fd(), &ev);
    ch->setInEpoll(true);
  }
}
// std::vector<epoll_event*>  Epoll::loop(int timeout){
//     std::vector<epoll_event> curevs;
//     int nums = epoll_wait(epollfd_,evs_,MAX_EVENTS,timeout);
//     if(nums==0){
//         //超时事件
//         printf("[SERVER] : Epoll Timeout!\n");
//         return curevs;
//     }else if(nums<0){//epoll失败
//         printf("File : %s, Function : %s,Line : %d, Errno :
//         %d\n",__FILE__,__FUNCTION__,__LINE__,errno); return curevs;
//     }
//     for(int i=0;i<nums;i++){
//         curevs.push_back(evs_[i]);
//     }
//     return curevs;
// }

std::vector<Channel *> Epoll::loop(int timeout) {
  std::vector<Channel *> channels;
  int nums = epoll_wait(epollfd_, evs_, MAX_EVENTS, timeout);
  if (nums == 0) {
    //超时事件
    // printf("[SERVER] : Epoll Timeout!\n");
    return channels;
  } else if (nums < 0) { // epoll失败
    printf("File : %s, Function : %s,Line : %d, Errno : %d\n", __FILE__,
           __FUNCTION__, __LINE__, errno);
    return channels;
  }
  for (int i = 0; i < nums; i++) {
    Channel *curChannel = ((Channel *)evs_[i].data.ptr);
    curChannel->setRevents(evs_[i].events);
    channels.push_back(curChannel);
  }
  return channels;
}
