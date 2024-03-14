#include "common.h"
#include "InetAddr.h"
#include "Socket.h"
#include "Epoll.h"
#include "EpollLoop.h"
#include "TcpServer.h"
#include <functional>
#include "EchoServer.h"
int main(int argc,char *argv[]){
    if(argc<3){
        printf("Usage : epoll_server ip port\n");
        exit(-1);
    }
    //获取ip与端口
    char *ip = argv[1];
    int port = atoi(argv[2]);
    EchoServer serv(ip,port);

    serv.start();
    


}