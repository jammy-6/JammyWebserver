#ifndef CONNECTION_H
#define CONNECTION_H
#include "Channel.h"
#include "EpollLoop.h"
#include "Socket.h"
#include <string>
#include <functional>
#include "Buffer.h"
#include <string>
#include "common.h"
class Connection{

private:
    
    EpollLoop *eventsLoop_; //从外面传入的的EpollLoop
    Channel *connectionChannel_;//与客户进行通信的channel
    Socket *cliSocket_; //客户端socket地址
    char buff_[1024];//当前channel读取数据，发送数据所用
    std::function<void(Socket *)> errorCallBack_; //当前连接发生错误时的回调函数
    std::function<void(Socket *)> closeCallBack_; //当前连接被关闭时的回调函数
    std::function<void(Connection *,std::string&)> messageCallBack_;
    std::function<void(Connection*)> sendCompleteCallBack_; //数据发送完毕时TCPSERVER的回调函数
    std::mutex updateWriteMutex_;
    Buffer inBuff_;
    Buffer outBuff_;
public:
    Connection(EpollLoop *eventsLoop,Socket *cliSocket);
    ~Connection();
    Socket * getCliSocket();
    void setErrorCallBack(std::function<void(Socket *)> callback);
    void setCloseCallBack(std::function<void(Socket *)> callback);
    void setMessageCallBack(std::function<void(Connection *,std::string&)> callback);
    void setSendCompleteCallBack(std::function<void(Connection *)> callback);
    void writeOutBuff(std::string &data);//TCPSERVER 类往输出缓冲区中添加数据
    void onMessageIn();
    void onMessageOut();
    void onClose();
    void sendMsg(std::string &data);
};



#endif