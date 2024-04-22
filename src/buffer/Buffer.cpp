#include "Buffer.h"


Buffer::Buffer(int initBuffSize) : buff_(initBuffSize,0), readPos_(0), writePos_(0) {}

size_t Buffer::ReadableBytes() const {
    return writePos_ - readPos_;
}
size_t Buffer::WritableBytes() const {
    return buff_.size() - writePos_;
}

size_t Buffer::PrependableBytes() const {
    return readPos_;
}

const char* Buffer::Peek() const {
    return BeginPtr_() + readPos_;
}

void Buffer::Retrieve(size_t len) {
    assert(len <= ReadableBytes());
    readPos_ += len;
}

void Buffer::RetrieveUntil(const char* end) {
    assert(Peek() <= end );
    Retrieve(end - Peek());
}

void Buffer::RetrieveAll() {
    bzero(&buff_[0], buff_.size());
    readPos_ = 0;
    writePos_ = 0;
}

std::string Buffer::RetrieveAllToStr() {
    std::string str(Peek(), ReadableBytes());
    RetrieveAll();
    return str;
}

const char* Buffer::BeginWriteConst() const {
    return BeginPtr_() + writePos_;
}

char* Buffer::BeginWrite() {
    return BeginPtr_() + writePos_;
}

void Buffer::HasWritten(size_t len) {
    writePos_ += len;
} 

void Buffer::Append(const std::string& str) {
    Append(str.data(), str.length());
}

void Buffer::Append(const void* data, size_t len) {
    assert(data);
    Append(static_cast<const char*>(data), len);
}

void Buffer::Append(const char* str, size_t len) {
    assert(str);
    EnsureWriteable(len);
    std::copy(str, str + len, BeginWrite());
    HasWritten(len);
}

void Buffer::Append(const Buffer& buff) {
    Append(buff.Peek(), buff.ReadableBytes());
}

void Buffer::EnsureWriteable(size_t len) {
    if(WritableBytes() < len) {
        MakeSpace_(len);
    }
    assert(WritableBytes() >= len);
}

ssize_t Buffer::ReadFd(int fd, int* saveErrno) {
    char buff[65535];
    struct iovec iov[2];
    const size_t writable = WritableBytes();
    /* 分散读， 保证数据全部读完 */
    iov[0].iov_base = BeginPtr_() + writePos_;
    iov[0].iov_len = writable;
    iov[1].iov_base = buff;
    iov[1].iov_len = sizeof(buff);

    const ssize_t len = readv(fd, iov, 2);
    if(len < 0) {
        *saveErrno = errno;
    }
    else if(static_cast<size_t>(len) <= writable) {
        writePos_ += len;
    }
    else {
        writePos_ = buff_.size();
        Append(buff, len - writable);
    }
    return len;
}

ssize_t Buffer::WriteFd(int fd, int* saveErrno) {
    size_t readSize = ReadableBytes();
    ssize_t len = write(fd, Peek(), readSize);
    if(len < 0) {
        *saveErrno = errno;
        return len;
    } 
    readPos_ += len;
    return len;
}

char* Buffer::BeginPtr_() {
    return &*buff_.begin();
}

const char* Buffer::BeginPtr_() const {
    return &*buff_.begin();
}

void Buffer::MakeSpace_(size_t len) {
    if(WritableBytes() + PrependableBytes() < len) {
        buff_.resize(writePos_ + len + 1);
    } 
    else {
        size_t readable = ReadableBytes();
        std::copy(BeginPtr_() + readPos_, BeginPtr_() + writePos_, BeginPtr_());
        readPos_ = 0;
        writePos_ = readPos_ + readable;
        assert(readable == ReadableBytes());
    }
}

//往buffer中添加数据
void Buffer::append(const char *data,int size){
    buff_.append(data,size);
}
//返回buff大小
int Buffer::size() {
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
    buff_.erase(0,size);
}

bool Buffer::getMsg(std::string &msg){
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

bool Buffer::getHttpMsg(std::string &msg){
    if(buff_.size()<2) return false;
    int idx = buff_.find("\r\n\r\n");
    if(idx!=std::string::npos){
        idx+=3;
    }
    if(idx==std::string::npos){
        idx = buff_.find("\n\n");
        if(idx!=std::string::npos){
            idx+=1;
        }
    }
    
    if(idx!=std::string::npos){
        msg = std::string(buff_.data(),idx+1);
        buff_.erase(0,idx+1);
        return true;
        
    }
    return false;
}