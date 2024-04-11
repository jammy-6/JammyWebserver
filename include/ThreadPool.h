#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <atomic>
#include <condition_variable>
#include <deque>
#include <functional>
#include <iostream>
#include <mutex>
#include <stdlib.h>
#include <thread>
#include <unistd.h>
#include <vector>
class ThreadPool {

private:
  //具体线程容器
  std::vector<std::thread> ths_;
  //任务队列
  std::deque<std::function<void()>> tasks_;
  //用于控制任务队列同步的互斥量和条件变量
  std::mutex mut_;
  std::condition_variable cv_;

  //线程池退出标志
  std::atomic_bool stop_;
  //线程池类型
  // IO类型：运行从Reactor模型调用的EventLoop::run函数
  // WORK类型，如由EchoServer对数据的计算操作
  std::string type_;

public:
  //构造函数
  ThreadPool(int size, std::string type);
  ~ThreadPool();
  //停止线程池
  void stop();
  //往任务队列中添加任务
  void addTask(std::function<void()> &&task);
  //线程从任务队列中取出任务执行
  // std::function<void()> & getTask();
};

#endif