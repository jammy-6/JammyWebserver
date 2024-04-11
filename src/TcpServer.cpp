#include "TcpServer.h"

//注意！！！！ eventLoops_必须在此处初始化
//若在下面调用 eventLoops_ = EventLoop()
//会导致epollfd被关闭，因为这里调用的是拷贝构造函数，然后会析构临时对象，造成问题
TcpServer::TcpServer(std::string ip, uint16_t port, int threadnum)
    : mainLoops_(new EventLoop(true, 3, 5)), threadnum_(threadnum),
      thp_(new ThreadPool(threadnum, "IO")),
      acceptor_(new Acceptor(ip, port, mainLoops_.get())) {

  acceptor_->setNewConnCallBack(
      std::bind(&TcpServer::newConnection, this, std::placeholders::_1));
  mainLoops_->setEpollTimeOutCallback(
      std::bind(&TcpServer::onEpollTimeOut, this, mainLoops_.get()));

  //往线程池中添加任务
  for (int i = 0; i < threadnum; i++) {
    subLoops_.emplace_back(new EventLoop(false, 5, 5));
    subLoops_[i]->setEpollTimeOutCallback(
        std::bind(&TcpServer::onEpollTimeOut, this, mainLoops_.get()));
    subLoops_[i]->setConnTimeoutCallBack(
        std::bind(&TcpServer::onConnTimeout, this, std::placeholders::_1));
    thp_->addTask(std::bind(&EventLoop::run, subLoops_[i].get()));
  }
}
TcpServer::~TcpServer() {

  //删除所有的连接客户
  // for(auto con:cons_){
  //     delete con.second;
  // }
}

void TcpServer::start() { mainLoops_->run(); }

void TcpServer::stop() {
  if (onServerStopCallback_)
    onServerStopCallback_();
  //销毁资源
  {
    std::lock_guard<std::mutex> guard(consMut_);
    for (auto &pair : cons_) {
      cons_.erase(pair.first);
    }
  }
  //关闭主从事件循环
  mainLoops_->stop();
  for (auto &loop : subLoops_) {
    loop->stop();
  }
}
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
      cliSocket, cliConnection); //将连接添加进EventLoop中的map
  if (onNewConnectionCallback_)
    onNewConnectionCallback_(cliSocket); //调用创建连接的回调函数
}

void TcpServer::connError(Socket *cliSocket) {

  if (onConnErrorCallback_) {
    onConnErrorCallback_(cliSocket);
  }
  {
    std::lock_guard<std::mutex> guard(consMut_);
    cons_.erase(cliSocket);
  }
}

void TcpServer::connClose(Socket *cliSocket) {
  if (onConnCloseCallback_) {
    onConnCloseCallback_(cliSocket);
  }
  {
    std::lock_guard<std::mutex> guard(consMut_);
    cons_.erase(cliSocket);
  }
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
void TcpServer::onEpollTimeOut(EventLoop *loop) {
  if (onEpollTimeOutCallback_) {
    onEpollTimeOutCallback_(loop);
  }
  // printf("[SERVER] : Epoll Time Out\n");
}

void TcpServer::onConnTimeout(Socket *cliSocket) {
  //调用回调函数
  if (onConnTimeOutCallback_)
    onConnTimeOutCallback_(cliSocket);
  //删除cons_中的无效连接
  {
    std::lock_guard<std::mutex> guard(consMut_);
    cons_.erase(cliSocket);
  }
}

//设置回调函数相关
void TcpServer::setOnNewConnectionCallback(
    std::function<void(Socket *)> callback) {
  onNewConnectionCallback_ = callback;
}
void TcpServer::setOnConnErrorCallback(std::function<void(Socket *)> callback) {
  onConnErrorCallback_ = callback;
}
void TcpServer::setOnConnCloseCallback(std::function<void(Socket *)> callback) {
  onConnCloseCallback_ = callback;
}
void TcpServer::setOnMessageCallback(
    std::function<void(spConnection con, std::string &data)> callback) {
  onMessageCallback_ = callback;
}
void TcpServer::setOnMsgSendCompleteCallback(
    std::function<void(spConnection)> callback) {

  onMsgSendCompleteCallback_ = callback;
}
void TcpServer::setOnEpollTimeOutCallback(
    std::function<void(EventLoop *)> callback) {

  onEpollTimeOutCallback_ = callback;
}

void TcpServer::setOnConnTimeOutCallback(
    std::function<void(Socket *)> callback) {
  onConnTimeOutCallback_ = callback;
}

void TcpServer::setOnServerStopCallback(std::function<void()> fun) {
  onServerStopCallback_ = fun;
}