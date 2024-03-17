#include "TcpServer.h"

//注意！！！！ eventLoops_必须在此处初始化
//若在下面调用 eventLoops_ = EpollLoop()
//会导致epollfd被关闭，因为这里调用的是拷贝构造函数，然后会析构临时对象，造成问题
TcpServer::TcpServer(std::string ip, uint16_t port, int threadnum)
    : mainLoops_(new EpollLoop(true, 3, 5)), threadnum_(threadnum),
      thp_(new ThreadPool(threadnum, "IO")),
      acceptor_(new Acceptor(ip, port, mainLoops_.get())) {

  acceptor_->setNewConnCallBack(
      std::bind(&TcpServer::newConnection, this, std::placeholders::_1));
  mainLoops_->setEpollTimeOutCallback(
      std::bind(&TcpServer::onEpollTimeOut, this, mainLoops_.get()));

  //往线程池中添加任务
  for (int i = 0; i < threadnum; i++) {
    subLoops_.emplace_back(new EpollLoop(false, 5, 5));
    subLoops_[i]->setEpollTimeOutCallback(
        std::bind(&TcpServer::onEpollTimeOut, this, mainLoops_.get()));
    subLoops_[i]->setConnTimeoutCallBack(
        std::bind(&TcpServer::removeConnect, this, std::placeholders::_1));
    thp_->addTask(std::bind(&EpollLoop::run, subLoops_[i].get()));
  }
}
TcpServer::~TcpServer() {

  //删除所有的连接客户
  // for(auto con:cons_){
  //     delete con.second;
  // }
}

void TcpServer::start() { mainLoops_->run(); }

void TcpServer::newConnection(Socket *cliSocket) {

  //在从事件循环中分配客户端连接
  spConnection cliConnection(
      new Connection(subLoops_[cliSocket->fd() % threadnum_].get(), cliSocket));

  cliConnection->setCloseCallBack(
      std::bind(&TcpServer::connClose, this, std::placeholders::_1));
  cliConnection->setErrorCallBack(
      std::bind(&TcpServer::connError, this, std::placeholders::_1));
  cliConnection->setMessageCallBack(std::bind(&TcpServer::onMessage, this,
                                              std::placeholders::_1,
                                              std::placeholders::_2));
  cliConnection->setSendCompleteCallBack(std::bind(
      &TcpServer::onMessageSendComplete, this, std::placeholders::_1));
  {
    std::lock_guard<std::mutex> guard(consMut_);
    cons_[cliSocket] = cliConnection; //将连接添加进TcpServer的map
  }

  subLoops_[cliSocket->fd() % threadnum_]->appendConn(
      cliSocket, cliConnection); //将连接添加进EpollLoop中的map
  if (onNewConnectionCallback_)
    onNewConnectionCallback_(cliSocket); //调用创建连接的回调函数
}

void TcpServer::connError(Socket *cliSocket) {
  if (onConnErrorCallback_) {
    onConnErrorCallback_(cliSocket);
  }

  cons_.erase(cliSocket);
}
void TcpServer::connClose(Socket *cliSocket) {
  if (onConnCloseCallback_) {
    onConnCloseCallback_(cliSocket);
  }

  cons_.erase(cliSocket);
}

void TcpServer::onMessage(spConnection con, std::string &data) {
  if (onMessageCallback_)
    onMessageCallback_(con, data);
}

void TcpServer::onMessageSendComplete(spConnection con) {
  if (con->getIsClose())
    return;
  if (onMsgSendCompleteCallback_) {
    onMsgSendCompleteCallback_(con);
  }
}
void TcpServer::onEpollTimeOut(EpollLoop *loop) {
  if (onEpollTimeOutCallback_) {
    onEpollTimeOutCallback_(loop);
  }
  printf("[SERVER] : Epoll Time Out\n");
}

//设置回调函数相关
void TcpServer::setOnNewConnectionCallback(std::function<void(Socket *)> fun) {
  onNewConnectionCallback_ = fun;
}
void TcpServer::setOnConnErrorCallback(std::function<void(Socket *)> fun) {
  onConnErrorCallback_ = fun;
}
void TcpServer::setOnConnCloseCallback(std::function<void(Socket *)> fun) {
  onConnCloseCallback_ = fun;
}
void TcpServer::setOnMessageCallback(
    std::function<void(spConnection con, std::string &data)> fun) {
  onMessageCallback_ = fun;
}
void TcpServer::setOnMsgSendCompleteCallback(
    std::function<void(spConnection)> fun) {
  onMsgSendCompleteCallback_ = fun;
}
void TcpServer::setOnEpollTimeOutCallback(
    std::function<void(EpollLoop *)> fun) {
  onEpollTimeOutCallback_ = fun;
}

void TcpServer::removeConnect(Socket *cliSocket) {
  {
    std::lock_guard<std::mutex> guard(consMut_);
    cons_.erase(cliSocket); //将连接添加进TcpServer的map
  }
}