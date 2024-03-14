#include "common.h"
#include "InetAddr.h"
#include "Socket.h"
#include "Epoll.h"

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
    //设置监听事件
    ep.addfd(listenSock.fd(),EPOLLIN); 
    char buff[1024];
    while(1){
        std::vector<epoll_event> evs = ep.loop();

        for(epoll_event ev:evs){
            if(ev.data.fd==listenSock.fd()){//处理客户端连接

                InetAddr cliaddr;
                int clifd = listenSock.accept(cliaddr);
                setnonblocking(clifd);
                Socket *cliSock = new Socket(clifd,cliaddr);
                printf("[SERVER] : Client Connect Success, Ip = %s, Port = %hu\n",cliaddr.ip(),cliaddr.port());

                ep.addfd(cliSock->fd(),EPOLLET|EPOLLIN);//边缘触发模式

            }else if(ev.events&EPOLLRDHUP){//客户端断开连接事件
                int disfd = ev.data.fd;
                close(disfd);
                printf("[SERVER] : Client Disconnect\n");
                epoll_ctl(disfd,EPOLL_CTL_DEL,disfd,nullptr);

            }else if(ev.events&EPOLLIN){//有数据到来
                int clientfd = ev.data.fd;
                while(1){
                    int num = recv(clientfd,buff,sizeof(buff),0);
                    if(num==0){//对方关闭连接
                        int disfd = ev.data.fd;
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

            }else if(ev.events&EPOLLOUT){//连接客户写事件

            }else{//其它待处理

            }
        }
    }
    


}