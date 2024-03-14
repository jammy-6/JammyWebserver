#ifndef BUFFER_H
#define BUFFER_H
#include <string>
#include <algorithm>
#include "common.h"
class Buffer{

private:
    std::string buff_;//buff数据
public:
    void append(char *data,int size);//往buffer中添加数据
    int size() const;//返回buff大小
    const char *ptr();//返回首地址指针
    void clear();
    void pop(int size);
    bool getMsg(std::string &msg);

};

#endif