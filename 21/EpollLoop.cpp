#include "EpollLoop.h"

EpollLoop::EpollLoop(bool isMainLoop,time_t clock,time_t timeout)
                :isMainLoop_(isMainLoop),clock_(clock),timeout_(timeout),ep_(new Epoll()),
                eventChannel_(new Channel(new Socket(eventfd(0,0),InetAddr()),this)),
                timerChannel_(new Channel(new Socket(timerfd_create(CLOCK_MONOTONIC,TFD_CLOEXEC|TFD_NONBLOCK),InetAddr()),this)){
    //把eventfd加入事件循环机制
    eventChannel_->enableReading();
    eventChannel_->setReadcallback(std::bind(&EpollLoop::handleEventChannelMsg,this));
    ep_->updateChannel(eventChannel_.get());

    if(!isMainLoop_){
        //设置timerfd的回调函数
        itimerspec tm;
        tm.it_value.tv_sec = clock_;
        tm.it_value.tv_nsec = 0;
        tm.it_interval.tv_sec = clock_;      //之后每次到期的时间间隔
        tm.it_interval.tv_nsec = 0;
        timerfd_settime(timerChannel_->fd(),0,&tm,0);
        timerChannel_->setReadcallback(std::bind(&EpollLoop::handleTimerChannelMsg,this));
        timerChannel_->enableReading();
        timerChannel_->setet();
        ep_->updateChannel(timerChannel_.get());
    }


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


//处理输出缓冲区任务
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


void EpollLoop::handleTimerChannelMsg(){
    uint64_t buf;
    int num = read(timerChannel_->fd(),&buf,sizeof(buf));
    printf("当前loop链接的客户：");
    {
        std::lock_guard<std::mutex> guard(timerMut_);
        for(auto &pair: cons_){
            if(!(pair.first)) break;
            printf("%d ",pair.first->fd());
            if(pair.second->isTimeOut(TimeStamp::now(),timeout_)){//如果超时
                cons_.erase(pair.first);
                connTimeoutCallBack(pair.first);
            }
        }
    }
    printf("\n");
    
}

void EpollLoop::appendConn(Socket * cliSocket,spConnection conn){
    {
        std::lock_guard<std::mutex> guard(timerMut_);
        cons_[cliSocket] = conn;
    }
}
void EpollLoop::removeConn(Socket * cliSocket){
    {
        std::lock_guard<std::mutex> guard(timerMut_);
        cons_.erase(cliSocket);
    }
    
}

void EpollLoop::setConnTimeoutCallBack(std::function<void(Socket *)> func){
    connTimeoutCallBack = func;
}