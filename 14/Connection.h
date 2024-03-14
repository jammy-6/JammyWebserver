#ifndef CONNECTION_H
#define CONNECTION_H
#include "Channel.h"
#include "EpollLoop.h"
#include "Socket.h"
#include <string>
#include <functional>
class Connection{

private:
    
    EpollLoop *eventsLoop_; //从外面传入的的EpollLoop
    Channel *connectionChannel_;//与客户进行通信的channel
    Socket *cliSocket_; //客户端socket地址
    char buff_[1024];//当前channel读取数据，发送数据所用
    std::function<void(Socket *)> errorCallBack_; //当前连接发生错误时的回调函数
    std::function<void(Socket *)> closeCallBack_; //当前连接被关闭时的回调函数
public:
    Connection(EpollLoop *eventsLoop,Socket *cliSocket);
    ~Connection();
    void setErrorCallBack(std::function<void(Socket *)> callback);
    void setCloseCallBack(std::function<void(Socket *)> callback);
    void onMessage();
};



#endif