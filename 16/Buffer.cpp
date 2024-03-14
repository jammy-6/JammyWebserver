#include "Buffer.h"


//往buffer中添加数据
void Buffer::append(char *data,int size){
    buff_.append(data,size);
}
//返回buff大小
int Buffer::size() const{
    return buff_.size();
}
//返回首地址指针
const char *Buffer::ptr(){
    return buff_.data();
}

void Buffer::clear(){
    buff_.clear();
}

void Buffer::pop(int size){
    buff_.erase(buff_.begin(),buff_.begin()+size),buff_.end();
}

bool Buffer::getMsg(std::string &msg){
    if(buff_.size()>=4){
        int len;
        memcpy(&len,buff_.data(),sizeof(int));
        if(buff_.size()>=4+len){
            msg = std::string(buff_.data()+4,len);
            pop(4+len);
            return true;
        }
    }
    return false;
}