#include "EchoServer.h"

EchoServer::EchoServer(std::string ip,uint16_t port):tcpServer_(ip,port){
    tcpServer_.setOnNewConnectionCallback(std::bind(&EchoServer::handleNewConnection,this,std::placeholders::_1));
    tcpServer_.setOnConnCloseCallback(std::bind(&EchoServer::handleConnClose,this,std::placeholders::_1));
    tcpServer_.setOnConnErrorCallback(std::bind(&EchoServer::handleConnError,this,std::placeholders::_1));
    tcpServer_.setOnMessageCallback(std::bind(&EchoServer::handleOnMessage,this,std::placeholders::_1,std::placeholders::_2));
    tcpServer_.setOnMsgSendCompleteCallback(std::bind(&EchoServer::handleOnMessageSendComplete,this,std::placeholders::_1));
}
EchoServer::~EchoServer(){

}



void EchoServer::handleNewConnection(Socket *cliSocket){
     printf("[SERVER] : Client Connect Success, Ip = %s, Port = %hu\n",cliSocket->getAddr()->ip(),cliSocket->getAddr()->port());
}
void EchoServer::handleConnError(Socket *cliSocket){
    printf("[SERVER] : Client Error %s:%d. Disconnect \n",cliSocket->getAddr()->ip(),cliSocket->getAddr()->port());
}
void EchoServer::handleConnClose(Socket *cliSocket){
    printf("[SERVER] : Client Close %s:%d. Disconnect \n",cliSocket->getAddr()->ip(),cliSocket->getAddr()->port());
    
}
void EchoServer::handleOnMessage(Connection *con,std::string &data){
    printf("[SERVER] : Recv : %s\n",data.data());
    con->writeOutBuff(data);
}
void EchoServer::handleOnMessageSendComplete(Connection *con){
    printf("[SERVER] : Connection %s:%d Send Complete\n",con->getCliSocket()->getAddr()->ip(),con->getCliSocket()->getAddr()->port());
}
void EchoServer::handleOnEpollTimeOut(EpollLoop* loop){

}

void EchoServer::start(){
    printf("[SERVER] EchoServer Start Successful\n");
    tcpServer_.start();
}
