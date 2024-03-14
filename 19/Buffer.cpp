#include "Buffer.h"


//往buffer中添加数据
void Buffer::append(char *data,int size){
    {
        std::lock_guard<std::mutex> guard(mut_);
        buff_.append(data,size);
    }
}
//返回buff大小
int Buffer::size() {
    std::lock_guard<std::mutex> guard(mut_);
    return buff_.size();
}
//返回首地址指针
const char *Buffer::ptr(){
    std::lock_guard<std::mutex> guard(mut_);
    return buff_.data();
}

void Buffer::clear(){
    std::lock_guard<std::mutex> guard(mut_);
    buff_.clear();
}

void Buffer::pop(int size){
    std::lock_guard<std::mutex> guard(mut_);
    buff_.erase(0,size);
}

bool Buffer::getMsg(std::string &msg){
    std::lock_guard<std::mutex> guard(mut_);
    if(buff_.size()>=4){
        int len;
        memcpy(&len,buff_.data(),sizeof(int));
        if(buff_.size()>=4+len){
            msg = std::string(buff_.data()+4,len);
            buff_.erase(0,4+len);
            return true;
        }
    }
    return false;
}
