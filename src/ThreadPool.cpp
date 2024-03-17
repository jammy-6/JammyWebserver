#include "ThreadPool.h"
#include <unistd.h>

/*
class ThreadPool{

private:
    //具体线程容器
    std::vector<std::thread> ths_;
    //任务队列
    std::deque<std::function<void()>> tasks_;
    //用于控制任务队列同步的互斥量和条件变量
    std::mutex mut_;
    std::condition_variable cv;

public:

    ThreadPool(int size);
    ~ThreadPool();

    //往任务队列中添加任务
    void addTask(std::function<void()> task);
    //线程从任务队列中取出任务执行
    std::function<void()> & getTask();


};
*/

#include <sys/syscall.h>
#include <unistd.h>
ThreadPool::ThreadPool(int size, std::string type) : stop_(false) {
  type_ = type;
  std::cout << "(" << type_ << ")"
            << "ThreadPoll With " << size << " Threads" << std::endl;

  for (int i = 0; i < size; i++) {
    //创建线程并执行线程函数
    ths_.emplace_back([this]() {
      std::cout << "(" << type_ << ")"
                << "Thread Id : " << syscall(SYS_gettid) << " Created"
                << std::endl;
      while (!stop_) {
        std::function<void()> task;
        //尝试从任务队列中取出任务
        {
          std::unique_lock<std::mutex> guard(mut_);
          cv_.wait(guard,
                   [this]() { return (stop_ == true) || (tasks_.size() > 0); });
          //当任务队列为空并且要终止线程池时线程才退出
          //如果任务队列非空先执行完任务再退出
          if (stop_ && tasks_.size() == 0)
            return;

          //正式获取任务
          task = std::move(tasks_.front());
          tasks_.pop_front();
        }
        std::cout << "(" << type_ << ")"
                  << "Thread Id : " << syscall(SYS_gettid) << " Excuted Task"
                  << std::endl;
        task();
      }
    });
  }
}

//线程池的析构函数
ThreadPool::~ThreadPool() {
  stop_ = true;
  cv_.notify_all(); //唤醒所有阻塞线程
  for (int i = 0; i < ths_.size(); i++) {
    ths_[i].join(); //等待所有子线程退出
  }
}

//往线程池的任务队列中添加任务
void ThreadPool::addTask(std::function<void()> &&task) {
  {
    std::unique_lock<std::mutex> guard(mut_);
    tasks_.push_back(std::move(task));
  }
  cv_.notify_one();
}

//测试函数
// int main(){
//     ThreadPool tp(3);
//     for(int i=0;i<10;i++){
//         tp.addTask([i](){
//             std::cout<<"大猫好"<<i<<std::endl;
//         });
//     }
//     tp.addTask([](){
//         std::cout<<"大猫好"<<std::endl;
//     });

//     tp.addTask([](){
//         std::cout<<"小猫好"<<std::endl;
//     });
//     sleep(1);
// }