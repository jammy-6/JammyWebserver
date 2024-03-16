#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "EpollLoop.h"
#include <string>
#include "Acceptor.h"
#include "Connection.h"
#include <map>
#include <string>

#include "ThreadPool.h"
#include <memory>
class TcpServer{
private:

    std::unique_ptr<EpollLoop> mainLoops_; //主事件循环，用于接受连接
    std::unique_ptr<Acceptor> acceptor_;//接受客户端请求
    std::mutex consMut_;//保护cons_，主事件循环写入时，从事件循环回调删除cons_操作
    std::map<Socket *,spConnection> cons_;//所有连接的客户端

    int threadnum_;//线程数量，用于从事件，负责客户通信
    std::unique_ptr<ThreadPool> thp_;//线程池
    std::vector<std::unique_ptr<EpollLoop>> subLoops_;//从事件循环

    std::function<void(Socket *)> onNewConnectionCallback_;//连接创建时的回调函数 
    std::function<void(Socket *)> onConnErrorCallback_;//连接创建时的回调函数 
    std::function<void(Socket *)> onConnCloseCallback_;//连接创建时的回调函数 
    std::function<void(spConnection,std::string &)> onMessageCallback_;//连接创建时的回调函数 
    std::function<void(spConnection)> onMsgSendCompleteCallback_;//连接创建时的回调函数 
    std::function<void(EpollLoop *)> onEpollTimeOutCallback_;//连接创建时的回调函数 

public:

    TcpServer(std::string ip,uint16_t port,int threadnum=3);
    ~TcpServer();
    void newConnection(Socket *cliSocket);//Acceptor类accept之后调用的回调函数
    void connError(Socket *cliSocket);//Connection类发生连接错误时调用的回调函数
    void connClose(Socket *cliSocket);//Connection类关闭连接时调用的回调函数
    void onMessage(spConnection con,std::string &data);//Connection类接受数据时调用的回调函数
    void onMessageSendComplete(spConnection );//Connection发送数据完毕时调用的回调函数
    void onEpollTimeOut(EpollLoop * loop);//EpollLoop类超时调用的回调函数
    void start();
    void removeConnect(Socket *cliSocket);

    //设置回调函数相关
    void setOnNewConnectionCallback(std::function<void(Socket *)> fun);//连接创建时的回调函数 
    void setOnConnErrorCallback(std::function<void(Socket *)> fun);//连接创建时的回调函数 
    void setOnConnCloseCallback(std::function<void(Socket *)> fun);//连接创建时的回调函数 
    void setOnMessageCallback(std::function<void(spConnection ,std::string &)> fun);//连接创建时的回调函数 
    void setOnMsgSendCompleteCallback(std::function<void(spConnection )> fun);//连接创建时的回调函数 
    void setOnEpollTimeOutCallback(std::function<void(EpollLoop*)> fun);//连接创建时的回调函数 


};

#endif