/*
 * @Author       : mark
 * @Date         : 2020-06-17
 * @copyleft Apache 2.0
 */

#include "sqlconnpool.h"
#include <string>
using namespace std;

SqlConnPool::SqlConnPool() {
  useCount_ = 0;
  freeCount_ = 0;
}

SqlConnPool *SqlConnPool::Instance() {
  static SqlConnPool connPool;
  return &connPool;
}

void SqlConnPool::Init(const char *host, int port, const char *user,
                       const char *pwd, const char *dbName, int connSize) {
  assert(connSize > 0);
  // ���� MySQL ����ʵ��
  sql::mysql::MySQL_Driver *driver = sql::mysql::get_mysql_driver_instance();

  for (int i = 0; i < connSize; i++) {
    std::string url = std::string("tcp://") + host + ":" + std::to_string(port);
    sql::Connection *sql = driver->connect(url, user, pwd);
    // sql::Connection *sql = nullptr;
    // sql = mysql_init(sql);
    if (!sql) {
      LOG_ERROR("MySql init error!");
      assert(sql);
    }
    // sql = mysql_real_connect(sql, host, user, pwd, dbName, port, nullptr, 0);
    sql->setSchema(dbName);
    if (!sql) {
      LOG_ERROR("MySql Connect error!");
    }
    connQue_.push(sql);
  }
  MAX_CONN_ = connSize;
  sem_init(&semId_, 0, MAX_CONN_);
}

sql::Connection *SqlConnPool::GetConn() {
  sql::Connection *sql = nullptr;
  if (connQue_.empty()) {
    LOG_WARN("SqlConnPool busy!");
    return nullptr;
  }
  sem_wait(&semId_);
  {
    lock_guard<mutex> locker(mtx_);
    sql = connQue_.front();
    connQue_.pop();
  }
  return sql;
}

void SqlConnPool::FreeConn(sql::Connection *sql) {
  assert(sql);
  lock_guard<mutex> locker(mtx_);
  connQue_.push(sql);
  sem_post(&semId_);
}

void SqlConnPool::ClosePool() {
  lock_guard<mutex> locker(mtx_);
  while (!connQue_.empty()) {
    auto item = connQue_.front();
    connQue_.pop();
    item->close();
    delete item;
  }
}

int SqlConnPool::GetFreeConnCount() {
  lock_guard<mutex> locker(mtx_);
  return connQue_.size();
}

SqlConnPool::~SqlConnPool() { ClosePool(); }
