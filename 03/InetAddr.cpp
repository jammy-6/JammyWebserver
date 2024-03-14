/*

class InetAddr{
public:
    InetAddr(const char*ip,short port);
    InetAddr(sockaddr_in addr_);
    const sockaddr *addr()const;
    const char *ip()const;
    short port()const;

private:
    sockaddr_in addr_;
};
*/

#include "InetAddr.h"

InetAddr::InetAddr(const char*ip,short port){
    bzero(&addr_,sizeof(addr_));
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(port);
    inet_pton(AF_INET,ip,&addr_.sin_addr);
}

InetAddr::InetAddr(sockaddr_in addr_):addr_(addr_){
    
}

const char *InetAddr::ip()const{
    return inet_ntoa(addr_.sin_addr);
}

int InetAddr::port()const{
    return ntohs(addr_.sin_port);
}

sockaddr *InetAddr::addr(){
    return (sockaddr *)&addr_;
}