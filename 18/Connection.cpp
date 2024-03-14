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
    connectionChannel_->setReadcallback(std::bind(&Connection::onMessageIn,this));
    connectionChannel_->setClosecallback(std::bind(&Connection::onMessageIn,this));
    connectionChannel_->setWritecallback(std::bind(&Connection::onMessageOut,this));
    connectionChannel_->setet();//边缘触发模式
    connectionChannel_->enableReading();
    //设置当输出缓存为空时的回调函数
    // 当输出缓存为空时，自动注销写事件
    // outBuff_.setEmptyBuffCallback(std::bind(&Channel::disableWriting,connectionChannel_));
}
Connection::~Connection(){
    delete connectionChannel_;
    delete cliSocket_;
}
//有数据到来
void Connection::onMessageIn(){
    while(1){
        int num = recv(connectionChannel_->fd(),buff_,sizeof(buff_),0);
        if(num==0){//对方关闭连接
            closeCallBack_(cliSocket_);
            return;
        }else if(num<0 && (errno==EAGAIN||errno==EWOULDBLOCK)){//内核缓冲区读取完毕
            std::string msg;
            if(inBuff_.getMsg(msg)){
                messageCallBack_(this,msg);//调用TCPSERVER回调函数处理消息
            }else{
                break;
            }
        }else if(num>0){
            //添加输入缓冲区数据
            inBuff_.append(buff_,num);
        }
    }
}
//TCPSERVER 类往输出缓冲区中添加数据
void Connection::writeOutBuff(std::string &data){
    
    char buff_[128];
    bzero(buff_,sizeof(buff_));
    int len = data.size();
    if(len<=0) //没有数据需要加入输出缓冲区
        return;
    memcpy(buff_,&len,sizeof(int));
    memcpy(buff_+sizeof(int),data.data(),len);

    std::lock_guard<std::mutex> guard(updateWriteMutex_);
    outBuff_.append(buff_,len+4);
    if((connectionChannel_->events()&EPOLLOUT)==false)//若epoll未监听写事件
        connectionChannel_->enableWriting();//让epollloop监听写事件
}

//Channel类监听到写事件，回调Connection发送数据
void Connection::onMessageOut(){
    std::lock_guard<std::mutex> guard(updateWriteMutex_);
    if(outBuff_.size()>0){
        int num = send(getCliSocket()->fd(),outBuff_.ptr(),outBuff_.size(),0);//将outBuff_中的数据尽可能多的发送出去
        if(num>0){
            outBuff_.pop(num);
        }
    }
    if(outBuff_.size()==0){//输出缓冲区发送完毕，取消监听写事件
        sendCompleteCallBack_(this);
        //这里有问题，先不管
        //多线程调用writeOutBuff函数时可能刚刚启用监听写事件，这里马上就禁用写事件
        //特别是在数据发送末尾阶段可能发生，导致outBuff_中还有数据未发送就被禁用写事件
        //即outBuff_.size()==0 与下面这个操作不能一气呵成导致的
        connectionChannel_->disableWriting();
    }
}

//客户端断开连接事件
void Connection::onClose(){
    closeCallBack_(cliSocket_);
}
void Connection::setErrorCallBack(std::function<void(Socket *)> callback){
    errorCallBack_ = callback;
}
void Connection::setCloseCallBack(std::function<void(Socket *)> callback){
    closeCallBack_ = callback;
}

Socket * Connection::getCliSocket(){
    return cliSocket_;
}

void Connection::setMessageCallBack(std::function<void(Connection *,std::string&)> callback){
    messageCallBack_ = callback;
}

void Connection::setSendCompleteCallBack(std::function<void(Connection *)> callback){
    sendCompleteCallBack_  = callback;
}