#include "Buffer.h"
using namespace std;

int main() {
  Buffer inBuff_;
  inBuff_.Append("1231231231\n\n233123123123");
  cout << inBuff_.Peek() << endl;
  cout << inBuff_.ReadableBytes() << endl;
  string msg;

  cout << inBuff_.getHttpMsg(msg) << endl;
  inBuff_.RetrieveAll();
  cout << inBuff_.ReadableBytes() << endl;
  return 0;
}