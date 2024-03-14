#include "EpollLoop.h"

EpollLoop::EpollLoop():ep_(new Epoll()),
                eventChannel_(new Channel(new Socket(eventfd(0,0),InetAddr()),this)){
    //把eventfd加入事件循环机制
    eventChannel_->enableReading();
    eventChannel_->setReadcallback(std::bind(&EpollLoop::handleEventChannelMsg,this));
    ep_->updateChannel(eventChannel_.get());

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

void EpollLoop::handleEventChannelMsg(){
    std::lock_guard<std::mutex> guard(mut_);
    
    std::function<void()> task;
    while(!taskque_.empty()){
        task = move(taskque_.front());
        taskque_.pop_front();
        task();
    }

}


//往任务队列中加入任务
void EpollLoop::addTask(std::function<void()> func){
    {
        std::lock_guard<std::mutex> guard(mut_);
        taskque_.push_back(std::move(func));
    }

    uint64_t ev;
    write(eventChannel_->fd(),&ev,sizeof(ev));
}