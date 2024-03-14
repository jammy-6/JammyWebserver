#include "TcpServer.h"


//注意！！！！ eventLoops_必须在此处初始化
//若在下面调用 eventLoops_ = EpollLoop() 会导致epollfd被关闭，因为这里调用的是拷贝构造函数，然后会析构临时对象，造成问题
TcpServer::TcpServer(std::string ip,uint16_t port):eventLoops_(EpollLoop()){
    acceptor_ = new Acceptor(ip,port,&eventLoops_);
    acceptor_->setNewConnCallBack(std::bind(&TcpServer::newConnection,this,std::placeholders::_1));
    eventLoops_.setEpollTimeOutCallback(std::bind(&TcpServer::onEpollTimeOut,this,std::placeholders::_1));
    // //创建监听socket,并设置为非阻塞

    // InetAddr servaddr(ip.c_str(),port);
    // Socket *listenSock = new Socket(createnonblocking(),servaddr);
    // listenSock->setReuseAddr(true);
    // listenSock->setTCPNODELAY(true);
    // listenSock->setKEEPALIVE(true);
    // listenSock->setREUSEPORT(true);
    // listenSock->bind();
    // listenSock->listen();

    // //创建epoll事件循环
    // //创建监听通道
    // Channel *listenChannel = new Channel(listenSock->fd(),eventLoops_.getEp());
    // listenChannel->setReadcallback(std::bind(&Channel::onAccept,listenChannel,listenSock));
    // //设置监听事件
    // listenChannel->enableReading();//一旦调用enableReading channel就会被epoll监听
    // // ep.updateChannel(listenChannel);

}
TcpServer::~TcpServer(){
    delete acceptor_;
    //删除所有的连接客户
    for(auto con:cons_){
        delete con.second;
    }
}

void TcpServer::start(){
    eventLoops_.run();
}


void TcpServer::newConnection(Socket *cliSocket){
   
    Connection *cliConnection = new Connection(&eventLoops_,cliSocket);
    cliConnection->setCloseCallBack(std::bind(&TcpServer::connClose,this,std::placeholders::_1));
    cliConnection->setErrorCallBack(std::bind(&TcpServer::connError,this,std::placeholders::_1));
    cliConnection->setMessageCallBack(std::bind(&TcpServer::onMessage,this,std::placeholders::_1,std::placeholders::_2));
    cliConnection->setSendCompleteCallBack(std::bind(&TcpServer::onMessageSendComplete,this,std::placeholders::_1));
    cons_[cliSocket] = cliConnection;
    printf("[SERVER] : Client Connect Success, Ip = %s, Port = %hu\n",cliSocket->getAddr()->ip(),cliSocket->getAddr()->port());

}

void TcpServer::connError(Socket *cliSocket){
    printf("[SERVER] : Client %s:%d Disconnect \n",cliSocket->getAddr()->ip(),cliSocket->getAddr()->port());
    delete cons_[cliSocket];
    cons_.erase(cliSocket);
}
void TcpServer::connClose(Socket *cliSocket){
    printf("[SERVER] : Client %s:%d Disconnect \n",cliSocket->getAddr()->ip(),cliSocket->getAddr()->port());
    delete cons_[cliSocket];
    cons_.erase(cliSocket);

}


void TcpServer::onMessage(Connection *con,std::string &data){
    printf("[SERVER] : Recv : %s\n",data.data());
    con->writeOutBuff(data);
}

void TcpServer::onMessageSendComplete(Connection *con){
    printf("[SERVER] : Connection %s:%d Send Complete\n",con->getCliSocket()->getAddr()->ip(),con->getCliSocket()->getAddr()->port());

}
void TcpServer::onEpollTimeOut(EpollLoop* loop){
    printf("[SERVER] : Epoll Time Out\n");
}