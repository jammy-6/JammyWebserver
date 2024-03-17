#ifndef TIME_STAMP_H
#define TIME_STAMP_H

#include <string>
#include <time.h>
class TimeStamp {
private:
  //自1970年开始的秒数
  time_t secsinceepoch_;

public:
  TimeStamp();
  TimeStamp(time_t sec);

  //返回当前时间的时间戳
  static TimeStamp now();
  //打赢时间戳
  std::string tostring() const;
  //返回secsinceepoch_
  time_t toint();
};

#endif