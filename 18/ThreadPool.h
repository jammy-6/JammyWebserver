#ifndef THREADPOOL_H
#define  THREADPOOL_H

#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>
#include <functional>
#include <iostream>
#include <deque>
#include <unistd.h>
#include <stdlib.h>

class ThreadPool{

private:
    //具体线程容器
    std::vector<std::thread> ths_;    
    //任务队列
    std::deque<std::function<void()>> tasks_;
    //用于控制任务队列同步的互斥量和条件变量
    std::mutex mut_;
    std::condition_variable cv_;

    //用于控制线程池退出
    bool stop_ = false;
    std::string type_;

public:

    ThreadPool(int size,std::string type);
    ~ThreadPool();

    //往任务队列中添加任务
    void addTask(std::function<void()> &&task);
    //线程从任务队列中取出任务执行
    // std::function<void()> & getTask();


};



#endif 