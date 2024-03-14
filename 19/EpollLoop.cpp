#include "EpollLoop.h"

EpollLoop::EpollLoop():ep_(new Epoll()){

}
EpollLoop::~EpollLoop(){
    
}
void EpollLoop::run(){
    while(1){
        std::vector<Channel*> channels  = ep_->loop(10*1000);//设置epoll_wait超时时长为10秒
        //遍历发生事件的信道
        if(channels.size()==0){
            epollTimeOutCallback_();
            continue;
        }
        for(Channel *ch:channels){
            //处理事件
            ch->handleEvent();
            //当用户退出时，用户通道会失效，对其进行删除
            // if(!ch->isValid()){
            //     delete ch;
            //     ch = nullptr;
            // }
        }
    }
}

Epoll * EpollLoop::getEp(){
    return ep_.get();
}

void EpollLoop::setEpollTimeOutCallback(std::function<void()> func){
    epollTimeOutCallback_ = func;
}