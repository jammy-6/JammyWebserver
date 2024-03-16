#ifndef BUFFER_H
#define BUFFER_H
#include <string>
#include <algorithm>
#include "common.h"
#include <mutex>
#include <functional>

class Buffer{

private:
    std::string buff_;//buff数据

public:
    // std::mutex mut_;//用于保护缓冲区
    void append(char *data,int size);//往buffer中添加数据
    int size() ;//返回buff大小
    const char *ptr();//返回首地址指针
    void clear();
    void pop(int size);
    bool getMsg(std::string &msg);


};

#endif