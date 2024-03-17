#ifndef BUFFER_H
#define BUFFER_H
#include "common.h"
#include <algorithm>
#include <functional>
#include <mutex>
#include <string>

class Buffer {

private:
  // buff实际储存的数据
  std::string buff_;

public:
  // std::mutex mut_;//用于保护缓冲区
  //往buffer中添加数据
  void append(char *data, int size);
  //返回buff大小
  int size();
  //返回首地址指针
  const char *ptr();
  //情况当前缓存
  void clear();
  //从buff_的头部取出size大小的数据
  void pop(int size);
  //从buff_中获取一条消息进行处理
  bool getMsg(std::string &msg);
};

#endif