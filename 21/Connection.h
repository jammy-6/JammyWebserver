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
#include <memory>
#include <atomic>
#include <iostream>
#include "TimeStamp.h"

class Channel;
class EpollLoop;
class Connection;
//Connection类的共享指针
using spConnection = std::shared_ptr<Connection>;

class Connection : public std::enable_shared_from_this<Connection>{

private:
    
    EpollLoop* eventsLoop_; //从外面传入的的EpollLoop
    std::unique_ptr<Channel> connectionChannel_;//与客户进行通信的channel
    std::unique_ptr<Socket> cliSocket_; //
    char buff_[1024];//当前channel读取数据，发送数据所用
    std::function<void(Socket *)> errorCallBack_; //当前连接发生错误时的回调函数
    std::function<void(Socket *)> closeCallBack_; //当前连接被关闭时的回调函数
    std::function<void(spConnection ,std::string&)> messageCallBack_;
    std::function<void(spConnection)> sendCompleteCallBack_; //数据发送完毕时TCPSERVER的回调函数
    std::atomic_bool isClose_;//判断当前连接是否关闭
    Buffer inBuff_;
    Buffer outBuff_;
    TimeStamp timeStamp_;//记录Connection最后一次更新的时间
public:
    Connection(EpollLoop *eventsLoop,Socket *cliSocket);
    ~Connection();
    Socket * getCliSocket();
    void setErrorCallBack(std::function<void(Socket *)> callback);
    void setCloseCallBack(std::function<void(Socket *)> callback);
    void setMessageCallBack(std::function<void(spConnection,std::string&)> callback);
    void setSendCompleteCallBack(std::function<void(spConnection )> callback);
    void writeOutBuff(std::string &data);//TCPSERVER 类往输出缓冲区中添加数据
    void onMessageIn();
    void onMessageOut();
    void onClose();
    void sendMsg(std::string &data);
    EpollLoop *getEventLoop();
    void removeChannel();
    bool getIsClose();
    void setIsClose(bool isClose);
    //判断连接是否超时
    bool isTimeOut(TimeStamp t,time_t timeout);
};



#endif