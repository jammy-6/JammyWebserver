#include "TimeStamp.h"
/*
Class TimeStamp{
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
*/

TimeStamp::TimeStamp(){
    secsinceepoch_ = time(nullptr);
    
}

TimeStamp::TimeStamp(time_t sec){
    secsinceepoch_=sec;
}

TimeStamp TimeStamp::now(){
    return TimeStamp(time(nullptr));
}
std::string TimeStamp::tostring()const{
    tm *t=localtime(&secsinceepoch_);
    
    char time[128];
    sprintf(time,"%04d-%02d-%02d- %02d:%02d:%02d",t->tm_year,t->tm_mon,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);
    return std::string(time);
}
time_t TimeStamp::toint(){
    return secsinceepoch_;
}
