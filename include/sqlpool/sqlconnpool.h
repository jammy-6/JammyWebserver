/*
 * @Author       : mark
 * @Date         : 2020-06-16
 * @copyleft Apache 2.0
 */
#ifndef SQLCONNPOOL_H
#define SQLCONNPOOL_H

#include "log.h"
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <mutex>
#include <mysql_connection.h>
#include <mysql_driver.h>
#include <queue>
#include <semaphore.h>
#include <string>
#include <thread>

class SqlConnPool {
public:
  static SqlConnPool *Instance();

  sql::Connection *GetConn();
  void FreeConn(sql::Connection *conn);
  int GetFreeConnCount();

  void Init(const char *host, int port, const char *user, const char *pwd,
            const char *dbName, int connSize = 10);
  void ClosePool();

private:
  SqlConnPool();
  ~SqlConnPool();

  int MAX_CONN_;
  int useCount_;
  int freeCount_;

  std::queue<sql::Connection *> connQue_;
  std::mutex mtx_;
  sem_t semId_;
};

#endif // SQLCONNPOOL_H