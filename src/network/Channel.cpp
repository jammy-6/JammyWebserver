#include "Channel.h"
#include "EventLoop.h"
#include <iostream>
/*
class Channel{
private:
    int fd_=-1;//channel类对应的fd，fd与channel是一对一关系
    Epoll *ep_;//channel类对应的Epoll指针， ep_与channel是一对多的关系
    bool isInEpoll=false;//判断当前channel是否被监听
    uint32_t events;//当前channel被epoll类监听的事件
    uint32_t revents;//当前channel在epoll_wait中实际发生的事件
public:
    Channel(int fd,Epoll *ep);
    ~Channel();
    void setInEpoll(bool on);
    void enableReading();
    bool inEpoll();
    void setRevents(uint32_t rev);
    uint32_t events();
    uint32_t revents();
};*/

Channel::Channel(Socket *sock, EventLoop *EventLoop)
    : sock_(sock), EventLoop_(EventLoop) {}
Channel::~Channel() {}
void Channel::setInEpoll(bool on) { isInEpoll_ = on; }
void Channel::enableReading() {
  events_ |= EPOLLIN;
  if (EventLoop_->getEp() != nullptr)
    EventLoop_->getEp()->updateChannel(this);
}

void Channel::enableET() {
  events_ |= EPOLLET;
  if (EventLoop_->getEp() != nullptr)
    EventLoop_->getEp()->updateChannel(this);
}
//设置监听写事件
void Channel::enableWriting() {
  events_ |= EPOLLOUT;
  if (EventLoop_->getEp() != nullptr)
    EventLoop_->getEp()->updateChannel(this);
}
//取消监听写事件
void Channel::disableWriting() {
  events_ &= ~EPOLLOUT;
  if (EventLoop_->getEp() != nullptr)
    EventLoop_->getEp()->updateChannel(this);
}

//屏蔽当前信道
void Channel::disableAll() {
  events_ &= 0;
  if (EventLoop_->getEp() != nullptr)
    EventLoop_->getEp()->updateChannel(this);
}
bool Channel::isInEpoll() const { return isInEpoll_; }
void Channel::setRevents(uint32_t rev) { revents_ = rev; }
uint32_t Channel::events() const { return events_; }
uint32_t Channel ::revents() const { return revents_; }
void Channel ::setValid(bool on) { valid_ = on; }
int Channel ::fd() const { return sock_->fd(); }

bool Channel::isValid() { return valid_; }

void Channel::setReadcallback(std::function<void()> callback) {
  readcallback_ = callback;
}

std::function<void()> Channel::getReadcallback() { return readcallback_; }

void Channel::setWritecallback(std::function<void()> callback) {
  writecallback_ = callback;
}
std::function<void()> Channel::getWritecallback() { return writecallback_; }
void Channel::setClosecallback(std::function<void()> callback) {
  closeCallBack_ = callback;
}
std::function<void()> Channel::getClosecallback() { return closeCallBack_; }

void Channel::handleEvent() {
  //读事件触发
  if (revents_ & EPOLLIN)
    readcallback_();
  else if (revents_ & EPOLLOUT){
    if (writecallback_) {
      writecallback_();
    } else {
      std::cout << "Callback function is null!" << std::endl;
    }
  }
    
  else if (revents_ & EPOLLRDHUP) {
    closeCallBack_();
  }
}
//用于接受客户端消息请求
// void Channel::onMessage(){
//     if(revents_&EPOLLRDHUP){//客户端断开连接事件
//         EventLoop_->getEp()->removeChannel(this);
//         return;
//     }else if(revents_&EPOLLIN){//有数据到来
//         while(1){
//             int num = recv(fd_,buff_,sizeof(buff_),0);
//             if(num==0){//对方关闭连接
//                 EventLoop_->getEp()->removeChannel(this);
//                 return;
//             }else if(num<0 &&
//             (errno==EAGAIN||errno==EWOULDBLOCK)){//内核缓冲区读取完毕
//                 break;
//             }else{
//                 printf("[SERVER] : Recv : %s\n",buff_);
//                 int num = send(fd_,buff_,sizeof(buff_),0);
//             }
//         }
//     }else if(revents_&EPOLLOUT){//连接客户写事件
//         return;
//     }else{//其它待处理
//         return;
//     }
// }
// #include "Connection.h"
//用于服务端接受客户端请求
// void Channel::onAccept(Socket *servSock){
//     InetAddr cliaddr;
//     int clifd = servSock->accept(cliaddr);
//     setnonblocking(clifd);
//     Socket *cliSock = new Socket(clifd,cliaddr);
//     printf("[SERVER] : Client Connect Success, Ip = %s, Port =
//     %hu\n",cliaddr.ip(),cliaddr.port()); Connection *cliConnection = new
//     Connection(EventLoop_,cliSock);
// }