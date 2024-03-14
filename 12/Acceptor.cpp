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

Acceptor::Acceptor( std::string &ip,u_int16_t port,EpollLoop *eventsLoop):eventsLoop_(eventsLoop){
    //创建监听socket,并设置为非阻塞

    InetAddr servaddr(ip.c_str(),port);
    servSocket_ = new Socket(createnonblocking(),servaddr);
    servSocket_->setReuseAddr(true);
    servSocket_->setTCPNODELAY(true);
    servSocket_->setKEEPALIVE(true);
    servSocket_->setREUSEPORT(true);
    servSocket_->bind();
    servSocket_->listen();

    //创建epoll事件循环
    //创建监听通道
    acceptChannel_ = new Channel(servSocket_,eventsLoop_);
    acceptChannel_->setReadcallback(std::bind(&Acceptor::newConnection,this));
    //设置监听事件
    acceptChannel_->enableReading();//一旦调用enableReading channel就会被epoll监听
    // ep.updateChannel(listenChannel);

}


Acceptor::~Acceptor(){
    delete acceptChannel_;
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
    printf("[SERVER] : Client Connect Success, Ip = %s, Port = %hu\n",cliaddr.ip(),cliaddr.port());
    newConnCallBack_(cliSock);//让tcpserver来做创建连接的操作

}