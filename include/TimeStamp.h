#ifndef TIME_STAMP_H
#define TIME_STAMP_H

#include <time.h>
#include <string>
class TimeStamp{
private:

//自1970年开始的秒数
time_t secsinceepoch_;

public:

TimeStamp();
TimeStamp(time_t sec);

static TimeStamp now();
std::string tostring()const;
time_t toint();

};

#endif