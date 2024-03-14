#include "Connection.h"

/*

class Connection{

private:
    
    EpollLoop *eventsLoop_; //从外面传入的的EpollLoop
    Channel *connectionChannel_;//与客户进行通信的channel
    Socket *cliSocket_; //服务端socket地址

public:
    Connection(EpollLoop *eventsLoop,Socket *cliSocket);
    ~Connection();

};*/

Connection::Connection(EpollLoop *eventsLoop,Socket *cliSocket):eventsLoop_(eventsLoop),cliSocket_(cliSocket){
    connectionChannel_ = new Channel(cliSocket,eventsLoop_);
    connectionChannel_->setReadcallback(std::bind(&Connection::onMessage,this));
    connectionChannel_->setet();//边缘触发模式
    connectionChannel_->enableReading();
}
Connection::~Connection(){
    delete connectionChannel_;
}

void Connection::onMessage(){
    if(connectionChannel_->revents()&EPOLLRDHUP){//客户端断开连接事件
        printf("[SERVER] : Client %s:%d Disconnect \n",cliSocket_->getAddr()->ip(),cliSocket_->getAddr()->port());
        eventsLoop_->getEp()->removeChannel(connectionChannel_);
        return;
    }else if(connectionChannel_->revents()&EPOLLIN){//有数据到来
        while(1){
            int num = recv(connectionChannel_->fd(),buff_,sizeof(buff_),0);
            if(num==0){//对方关闭连接
                printf("[SERVER] : Client %s:%d Disconnect \n",cliSocket_->getAddr()->ip(),cliSocket_->getAddr()->port());
                eventsLoop_->getEp()->removeChannel(connectionChannel_);
                return;
            }else if(num<0 && (errno==EAGAIN||errno==EWOULDBLOCK)){//内核缓冲区读取完毕
                break;
            }else{
                printf("[SERVER] : Recv : %s\n",buff_);
                int num = send(connectionChannel_->fd(),buff_,sizeof(buff_),0);
            }
        }
    }else if(connectionChannel_->revents()&EPOLLOUT){//连接客户写事件
        return;
    }else{//其它待处理
        return;
    }
}