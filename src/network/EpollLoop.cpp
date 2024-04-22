#include "EventLoop.h"
#include "Socket.h"
#include <vector>

EventLoop::EventLoop(bool isMainLoop, time_t clock, time_t timeout)
    : isMainLoop_(isMainLoop), clock_(clock), timeout_(timeout),
      ep_(new Epoll()), connOutBuffChannel_(new Channel(
                            new Socket(eventfd(0, 0), InetAddr()), this)),
      timerChannel_(
          new Channel(new Socket(timerfd_create(CLOCK_MONOTONIC,
                                                TFD_CLOEXEC | TFD_NONBLOCK),
                                 InetAddr()),
                      this)) {
  //把eventfd加入事件循环机制
  connOutBuffChannel_->enableReading();
  connOutBuffChannel_->enableET();
  connOutBuffChannel_->setReadcallback(
      std::bind(&EventLoop::handleConnOutbufflMsg, this));
  ep_->updateChannel(connOutBuffChannel_.get());

  if (!isMainLoop_) {
    //设置timerfd的回调函数
    itimerspec tm;
    tm.it_value.tv_sec = clock_;
    tm.it_value.tv_nsec = 0;
    tm.it_interval.tv_sec = clock_; //之后每次到期的时间间隔
    tm.it_interval.tv_nsec = 0;
    timerfd_settime(timerChannel_->fd(), 0, &tm, 0);
    timerChannel_->setReadcallback(
        std::bind(&EventLoop::handleTimerChannelMsg, this));
    timerChannel_->enableReading();
    timerChannel_->enableET();
  }
}
EventLoop::~EventLoop() {}
void EventLoop::run() {
  status_ = true;
  while (status_) {
    std::vector<Channel *> channels =
        ep_->loop(10 * 1000); //设置epoll_wait超时时长为10秒
    //遍历发生事件的信道
    if (channels.size() == 0) {
      epollTimeOutCallback_();
      continue;
    }
    for (Channel *ch : channels) {
      //处理事件
      ch->handleEvent();
    }
  }
}

void EventLoop::stop() { status_ = false; }
Epoll *EventLoop::getEp() { return ep_.get(); }

void EventLoop::setEpollTimeOutCallback(std::function<void()> func) {
  epollTimeOutCallback_ = func;
}

//处理输出缓冲区任务
void EventLoop::handleConnOutbufflMsg() {
  std::lock_guard<std::mutex> guard(mut_);

  std::function<void()> task;
  while (!taskque_.empty()) {
    task = move(taskque_.front());
    taskque_.pop_front();
    task();
  }
}

//往任务队列中加入任务
void EventLoop::addTask(std::function<void()> func) {
  {
    std::lock_guard<std::mutex> guard(mut_);
    taskque_.push_back(std::move(func));
  }

  uint64_t ev;
  write(connOutBuffChannel_->fd(), &ev, sizeof(ev));
}

void EventLoop::handleTimerChannelMsg() {
  uint64_t buf;
  int num = read(timerChannel_->fd(), &buf, sizeof(buf));
  std::vector<Socket *> deleConSocks;
  for (auto &pair : cons_) {
    if (!(pair.first))
      break;
    if (pair.second->isTimeOut(TimeStamp::now(), timeout_)) { //如果超时
      deleConSocks.push_back(pair.first);
    }
  }

  {
    std::lock_guard<std::mutex> guard(timerMut_);
    for (Socket *sock : deleConSocks) {
      cons_.erase(sock);
      connTimeoutCallBack_(sock);
    }
  }
}

void EventLoop::appendConn(Socket *cliSocket, spConnection conn) {
  {
    std::lock_guard<std::mutex> guard(timerMut_);
    cons_[cliSocket] = conn;
  }
}
void EventLoop::removeConn(Socket *cliSocket) {
  {
    std::lock_guard<std::mutex> guard(timerMut_);
    cons_.erase(cliSocket);
  }
}

void EventLoop::setConnTimeoutCallBack(std::function<void(Socket *)> func) {
  connTimeoutCallBack_ = func;
}