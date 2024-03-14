#include "Socket.h"

int setnonblocking(int fd){
    int old = fcntl(fd,F_GETFL);
    fcntl(fd,F_SETFL,old|SOCK_NONBLOCK);
    return old;
}

int createnonblocking(){
    int listenfd = socket(AF_INET,SOCK_STREAM|SOCK_NONBLOCK,IPPROTO_TCP);
    if(listenfd<0){
        printf("File : %s, Function : %s,Line : %d, Errno : %d\n",__FILE__,__FUNCTION__,__LINE__,errno);
        exit(-1);
    }
    setnonblocking(listenfd);
    return listenfd;
}

/*
class Socket
{
private:
    int fd_;//封装的socket
    InetAddr servaddr_;//socket所对应的地址
public:
    Socket(int fd_); //构造函数
    ~Socket();

    void setAddr(InetAddr addr);

    void bind(InetAddr addr);
    void listen(int blog=128);
    int accept();
    void setReuseAddr(bool on);
    void setTCPNODELAY(bool on);
    void setKEEPALIVE(bool on);
    void setREUSEPORT(bool on);


};

*/
Socket::Socket(){

}
Socket::Socket(int fd,InetAddr addr):fd_(fd),addr_(addr){

}
Socket::~Socket(){
    close(fd_);
}
int Socket::fd()const{
    return fd_;
}
void Socket::setAddr(InetAddr addr){
    addr_=addr;
}


void Socket::bind(){
    int ret = ::bind(fd_,addr_.addr(),sizeof(struct sockaddr));
    if(ret!=0){
        printf("File : %s, Function : %s,Line : %d, Errno : %d\n",__FILE__,__FUNCTION__,__LINE__,errno);
        exit(-1);
    }
}

void Socket::listen(int blog){
    int ret  = ::listen(fd_,blog);
    if(ret!=0){
        printf("File : %s, Function : %s,Line : %d, Errno : %d\n",__FILE__,__FUNCTION__,__LINE__,errno);
        exit(-1);
    }
}

int Socket::accept(InetAddr &addr){
    sockaddr_in peerAddr;
    bzero(&peerAddr,sizeof(sockaddr_in));
    int len = sizeof(peerAddr);
    int clientfd = ::accept(fd_,(sockaddr*)&peerAddr,(socklen_t*)(&len));
    addr = InetAddr(peerAddr);
    if(clientfd<0){
        printf("File : %s, Function : %s,Line : %d, Errno : %d\n",__FILE__,__FUNCTION__,__LINE__,errno);
    }
    return clientfd;
}

void Socket::setReuseAddr(bool on){
    int opt = on?1:0;
    setsockopt(fd_,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
}
void Socket::setTCPNODELAY(bool on){
    int opt = on?1:0;
    setsockopt(fd_,IPPROTO_TCP,TCP_NODELAY,&opt,sizeof(opt));
}
void Socket::setKEEPALIVE(bool on){
    int opt = on?1:0;
    setsockopt(fd_,SOL_SOCKET,SO_KEEPALIVE,&opt,sizeof(opt));
}
void Socket::setREUSEPORT(bool on){
    int opt = on?1:0;
    setsockopt(fd_,SOL_SOCKET,SO_REUSEPORT,&opt,sizeof(opt));
}