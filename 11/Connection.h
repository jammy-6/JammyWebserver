#ifndef CONNECTION_H
#define CONNECTION_H
#include "Channel.h"
#include "EpollLoop.h"
#include "Socket.h"
#include <string>

class Connection{

private:
    
    EpollLoop *eventsLoop_; //从外面传入的的EpollLoop
    Channel *connectionChannel_;//与客户进行通信的channel
    Socket *cliSocket_; //服务端socket地址

public:
    Connection(EpollLoop *eventsLoop,Socket *cliSocket);
    ~Connection();

};



#endif