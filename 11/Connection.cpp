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
    connectionChannel_ = new Channel(cliSocket->fd(),eventsLoop_);
    connectionChannel_->setReadcallback(std::bind(&Channel::onMessage,connectionChannel_));
    connectionChannel_->setet();//边缘触发模式
    connectionChannel_->enableReading();
}
Connection::~Connection(){
    delete connectionChannel_;
    delete cliSocket_;
}