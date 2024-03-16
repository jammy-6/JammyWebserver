#include "Acceptor.h"

/*
class Acceptor{

private:
    
    EpollLoop *eventsLoop_; //从外面传入的的EpollLoop
    Channel *acceptChannel_;//用于接受客户连接请求的channel
    Socket *servSocket_; //服务端socket地址

public:
    Acceptor( std::string &ip,u_int16_t port,EpollLoop *eventsLoop_);
    ~Acceptor();
    
};
*/

Acceptor::Acceptor( std::string &ip,u_int16_t port,EpollLoop *eventsLoop)
:eventsLoop_(eventsLoop),
servSocket_(new Socket(createnonblocking(),InetAddr(ip.c_str(),port))),
acceptChannel_(Channel(servSocket_,eventsLoop_)){
    //创建监听socket,并设置为非阻塞

    servSocket_->setReuseAddr(true);
    servSocket_->setTCPNODELAY(true);
    servSocket_->setKEEPALIVE(true);
    servSocket_->setREUSEPORT(true);
    servSocket_->bind();
    servSocket_->listen();

    //创建epoll事件循环

    acceptChannel_.setReadcallback(std::bind(&Acceptor::newConnection,this));
    //设置监听事件
    acceptChannel_.enableReading();//一旦调用enableReading channel就会被epoll监听
    // ep.updateChannel(listenChannel);

}


Acceptor::~Acceptor(){
    delete servSocket_;
}

void Acceptor::setNewConnCallBack(std::function<void(Socket *)> func){
    newConnCallBack_ = func;
}

void Acceptor::newConnection(){
    InetAddr cliaddr;
    int clifd = servSocket_->accept(cliaddr);
    setnonblocking(clifd);
    Socket *cliSock = new Socket(clifd,cliaddr);
    
    newConnCallBack_(cliSock);//让tcpserver来做创建连接的操作

}