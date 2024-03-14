#include "Channel.h"

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


Channel::Channel(Socket *sock,EpollLoop *epollLoop):sock_(sock),epollLoop_(epollLoop){

}
Channel::~Channel(){

}
void Channel::setInEpoll(bool on){
    isInEpoll_ = on;
}
void Channel::enableReading(){
    events_ |=EPOLLIN;
    if(epollLoop_->getEp()!=nullptr)
        epollLoop_->getEp()->updateChannel(this);
    
}

void Channel::setet(){
    events_|=EPOLLET;
    if(epollLoop_->getEp()!=nullptr)
        epollLoop_->getEp()->updateChannel(this);
    
}
bool Channel::inEpoll()const{
    return isInEpoll_;
}
void Channel::setRevents(uint32_t rev){
    revents_ = rev;
}
uint32_t Channel::events()const{
    return events_;
}
uint32_t Channel ::revents()const{
    return revents_;
}
void Channel ::setValid(bool on){
    valid_ = on;
}
int Channel ::fd()const{
    return sock_->fd();
}

bool Channel::isValid(){
    return valid_;
}


void Channel::setReadcallback(std::function<void()> callback){
    readcallback_  = callback;
}

std::function<void()> Channel::getReadcallback(){
    return readcallback_;
}

void Channel::handleEvent(){
    readcallback_();
}
//用于接受客户端消息请求
// void Channel::onMessage(){
//     if(revents_&EPOLLRDHUP){//客户端断开连接事件
//         epollLoop_->getEp()->removeChannel(this);
//         return;
//     }else if(revents_&EPOLLIN){//有数据到来
//         while(1){
//             int num = recv(fd_,buff_,sizeof(buff_),0);
//             if(num==0){//对方关闭连接
//                 epollLoop_->getEp()->removeChannel(this);
//                 return;
//             }else if(num<0 && (errno==EAGAIN||errno==EWOULDBLOCK)){//内核缓冲区读取完毕
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
//     printf("[SERVER] : Client Connect Success, Ip = %s, Port = %hu\n",cliaddr.ip(),cliaddr.port());
//     Connection *cliConnection = new Connection(epollLoop_,cliSock);
// }