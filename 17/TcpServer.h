#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "EpollLoop.h"
#include <string>
#include "Acceptor.h"
#include "Connection.h"
#include <map>
#include <string>
class TcpServer{
private:

    EpollLoop eventLoops_; //一个TcpServer对应一个事件循环
    Acceptor *acceptor_;//接受客户端请求
    std::map<Socket *,Connection*> cons_;//所有连接的客户端

    std::function<void(Socket *)> onNewConnectionCallback_;//连接创建时的回调函数 
    std::function<void(Socket *)> onConnErrorCallback_;//连接创建时的回调函数 
    std::function<void(Socket *)> onConnCloseCallback_;//连接创建时的回调函数 
    std::function<void(Connection *,std::string &)> onMessageCallback_;//连接创建时的回调函数 
    std::function<void(Connection *)> onMsgSendCompleteCallback_;//连接创建时的回调函数 
    std::function<void(EpollLoop *)> onEpollTimeOutCallback_;//连接创建时的回调函数 

public:

    TcpServer(std::string ip,uint16_t port);
    ~TcpServer();
    void newConnection(Socket *cliSocket);//Acceptor类accept之后调用的回调函数
    void connError(Socket *cliSocket);//Connection类发生连接错误时调用的回调函数
    void connClose(Socket *cliSocket);//Connection类关闭连接时调用的回调函数
    void onMessage(Connection *con,std::string &data);//Connection类接受数据时调用的回调函数
    void onMessageSendComplete(Connection *);//Connection发送数据完毕时调用的回调函数
    void onEpollTimeOut(EpollLoop* loop);//EpollLoop类超时调用的回调函数
    void start();

    //设置回调函数相关
    void setOnNewConnectionCallback(std::function<void(Socket *)> fun);//连接创建时的回调函数 
    void setOnConnErrorCallback(std::function<void(Socket *)> fun);//连接创建时的回调函数 
    void setOnConnCloseCallback(std::function<void(Socket *)> fun);//连接创建时的回调函数 
    void setOnMessageCallback(std::function<void(Connection *con,std::string &data)> fun);//连接创建时的回调函数 
    void setOnMsgSendCompleteCallback(std::function<void(Connection *)> fun);//连接创建时的回调函数 
    void setOnEpollTimeOutCallback(std::function<void(EpollLoop *)> fun);//连接创建时的回调函数 


};

#endif