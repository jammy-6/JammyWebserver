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

InetAddr::InetAddr(){

}

const char *InetAddr::ip()const{
    return inet_ntoa(addr_.sin_addr);
}

u_int16_t InetAddr::port()const{
    u_int16_t port = ntohs(addr_.sin_port);
    return port;
}

sockaddr *InetAddr::addr(){
    return (sockaddr *)&addr_;
}