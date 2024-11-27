#ifndef CHANNEL_H
#define CHANNEL_H

#include "Socket.h"
#include <functional>
class EventLoop;

class Channel {
private:
  // int fd_=-1;//channel类对应的fd，fd与channel是一对一关系
  Socket *sock_;
  // channel类对应的Epoll指针，ep_与channel是一对多的关系
  EventLoop *EventLoop_;
  // 判断当前channel是否向内核注册事件
  bool isInEpoll_ = false;
  // 当前channel被epoll类监听的事件
  uint32_t events_ = 0;
  // 当前channel在epoll_wait中实际发生的事件
  uint32_t revents_ = 0;
  // 当前信道是否有效，ep->removeChannel时标记为false
  bool valid_ = true;
  // 发生读事件时的回调函数
  std::function<void()> readcallback_;
  // 发生写事件时的回调函数
  std::function<void()> writecallback_;
  // 信道由于错误或EPOLLRDHUP事件发生时的回调函数
  std::function<void()> closeCallBack_;

public:
  // 信道构造函数
  Channel(Socket *sock, EventLoop *EventLoop);
  // 信道析构函数
  ~Channel();

  // 判断当前信道是否有效
  bool isValid();
  // 设置当前信道有效性
  void setValid(bool on);

  // 设置当前信道状态为被内核监听
  void setInEpoll(bool on);
  // 返回当前信道的监听状态
  bool isInEpoll() const;

  // 设置边缘触发模式
  void enableET();
  // 设置监听读事件
  void enableReading();
  // 设置监听写事件
  void enableWriting();
  // 取消监听写事件
  void disableWriting();
  // 取消监听所有事件
  void disableAll();
  // 设置当前信道实际发生事件，由Epoll::loop函数调用
  void setRevents(uint32_t rev);

  // 当前信道监听的事件
  uint32_t events() const;
  // 当前信道实际发生的事件
  uint32_t revents() const;
  // 返回当前信道中sock_的地址
  int fd() const;

  // 事件分发函数，对revents中的事件分发给回调函数
  void handleEvent();

  // 获取和设置相关回调函数
  std::function<void()> getReadcallback();
  std::function<void()> getWritecallback();
  std::function<void()> getClosecallback();
  void setReadcallback(std::function<void()> callback);
  void setWritecallback(std::function<void()> callback);
  void setClosecallback(std::function<void()> callback);

  // void onMessage();//用于接受客户端消息请求
  // void onAccept(Socket *servSock);//用于服务端接受客户端请求
};

#endif