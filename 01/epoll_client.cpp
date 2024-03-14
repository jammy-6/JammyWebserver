#include "common.h"

int main(int argc,char *argv[]){
    if(argc<3){
        printf("Usage : epoll_client ip port");
        exit(-1);
    }   

    
    int clisock = socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in servaddr;
    bzero(&servaddr,sizeof(sockaddr_in));
    servaddr.sin_family=AF_INET;
    servaddr.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET,argv[1],&servaddr.sin_addr);

    connect(clisock,(sockaddr*)&servaddr,sizeof(sockaddr_in));
    char buff[1024];

    while(1){
        printf("[CLIENT] : Input ");
        scanf("%s",buff);
        int num  = send(clisock,buff,sizeof(buff),0);
        bzero(buff,sizeof(buff));
        num = recv(clisock,buff,sizeof(buff),0);
        printf("[CLIENT] : Receive %s\n",buff);
    }

}