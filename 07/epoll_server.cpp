#include "common.h"
#include "InetAddr.h"
#include "Socket.h"
#include "Epoll.h"
#include <functional>
int main(int argc,char *argv[]){
    if(argc<3){
        printf("Usage : epoll_server ip port\n");
        exit(-1);
    }
    //获取ip与端口
    char *ip = argv[1];
    int port = atoi(argv[2]);

    //创建监听socket,并设置为非阻塞
    InetAddr servaddr(ip,port);
    Socket listenSock(createnonblocking(),servaddr);
    listenSock.setReuseAddr(true);
    listenSock.setTCPNODELAY(true);
    listenSock.setKEEPALIVE(true);
    listenSock.setREUSEPORT(true);
    listenSock.bind();
    listenSock.listen();
    
    //创建epoll事件循环
    Epoll ep = Epoll();

    //创建监听通道
    Channel *listenChannel = new Channel(listenSock.fd(),&ep);
    listenChannel->setReadcallback(std::bind(&Channel::onAccept,listenChannel,&listenSock));
    //设置监听事件
    listenChannel->enableReading();//一旦调用enableReading channel就会被epoll监听
    // ep.updateChannel(listenChannel);
    char buff[1024];
    while(1){
        std::vector<Channel*> channels  = ep.loop();
        //遍历发生事件的信道
        for(Channel *ch:channels){
            //处理事件
            ch->handleEvent();
            //当用户退出时，用户通道会失效，对其进行删除
            if(!ch->isValid()){
                delete ch;
                ch = nullptr;
            }
        }
    }
    


}