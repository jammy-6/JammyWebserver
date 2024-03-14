#include "common.h"
#include "InetAddr.h"
#include "Socket.h"


int main(int argc,char *argv[]){
    if(argc<3){
        printf("Usage : epoll_server ip port\n");
        exit(-1);
    }
    //获取ip与端口
    char *ip = argv[1];
    int port = atoi(argv[2]);

    //创建监听socket,并设置为非阻塞
    // int listenfd = socket(AF_INET,SOCK_STREAM|SOCK_NONBLOCK,IPPROTO_TCP);
    // int opt = 1;
    // setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
    // setsockopt(listenfd,IPPROTO_TCP,TCP_NODELAY,&opt,sizeof(opt));
    // setsockopt(listenfd,SOL_SOCKET,SO_KEEPALIVE,&opt,sizeof(opt));
    // setsockopt(listenfd,SOL_SOCKET,SO_REUSEPORT,&opt,sizeof(opt));
    // struct sockaddr_in servaddr;
    // bzero(&servaddr,sizeof(servaddr));
    // servaddr.sin_family = AF_INET;
    // servaddr.sin_port = htons(port);
    // inet_pton(AF_INET,ip,&servaddr.sin_addr);

    InetAddr servaddr(ip,port);
    Socket listenSock(createnonblocking(),servaddr);
    listenSock.setReuseAddr(true);
    listenSock.setTCPNODELAY(true);
    listenSock.setKEEPALIVE(true);
    listenSock.setREUSEPORT(true);
    listenSock.bind();
    listenSock.listen();
    
    //创建epoll事件循环
    int epollfd;
    epollfd = epoll_create(16);
    //设置监听事件
    struct epoll_event ev;
    ev.data.fd = listenSock.fd();
    ev.events = (EPOLLET|EPOLLIN);
    
    epoll_ctl(epollfd,EPOLL_CTL_ADD,listenSock.fd(),&ev);

    epoll_event max_events[16];
    bzero(&max_events,sizeof(max_events));
    
    char buff[1024];
    while(1){
        int nums = epoll_wait(epollfd,max_events,16,-1);
        if(nums==0){
            //超时事件
            continue;
        }else if(nums<0){
            break;
        }

        for(int i=0;i<nums;i++){
            if(max_events[i].data.fd==listenSock.fd()){//处理客户端连接

                InetAddr cliaddr;
                int clifd = listenSock.accept(cliaddr);
                setnonblocking(clifd);
                Socket *cliSock = new Socket(clifd,cliaddr);
                printf("[SERVER] : Client Connect Success, Ip = %s, Port = %hu\n",cliaddr.ip(),cliaddr.port());

                ev.data.fd = cliSock->fd();
                ev.events|=EPOLLET|EPOLLIN;//边缘触发模式
                epoll_ctl(epollfd,EPOLL_CTL_ADD,cliSock->fd(),&ev);

            }else if(max_events[i].events&EPOLLRDHUP){//客户端断开连接事件

                int disfd = max_events[i].data.fd;
                close(disfd);
                printf("[SERVER] : Client Disconnect\n");
                epoll_ctl(disfd,EPOLL_CTL_DEL,disfd,nullptr);

            }else if(max_events[i].events&EPOLLIN){//有数据到来
                int clientfd = max_events[i].data.fd;
                while(1){
                    int num = recv(clientfd,buff,sizeof(buff),0);
                    if(num==0){//对方关闭连接
                        int disfd = max_events[i].data.fd;
                        close(disfd);
                        printf("[SERVER] : Client Disconnect\n");

                        epoll_ctl(disfd,EPOLL_CTL_DEL,disfd,nullptr);
                        break;
                    }else if(num<0 && (errno==EAGAIN||errno==EWOULDBLOCK)){//内核缓冲区读取完毕
                        break;
                    }else{
                        printf("[SERVER] : Recv : %s\n",buff);
                        int num = send(clientfd,buff,sizeof(buff),0);

                    }
                }

            }else if(max_events[i].events&EPOLLOUT){//连接客户写事件

            }else{//其它待处理

            }
        }
    }
    


}