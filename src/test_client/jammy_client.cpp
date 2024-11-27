#include "Buffer.h"
#include "common.h"
// int setnonblocking(int fd){
//     int old = fcntl(fd,F_GETFL);
//     fcntl(fd,F_SETFL,old|SOCK_NONBLOCK);
//     return old;
// }

Buffer inBuff;
int main(int argc, char *argv[]) {
  time_t curTime = time(nullptr);
  char filename[128];
  // sprintf(filename,"%s%ld%s","output",curTime%100,".txt");
  // FILE *fp = freopen(filename, "w", stdout);
  if (argc < 3) {
    printf("Usage : epoll_client ip port\n");
    exit(-1);
  }

  int clisock = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in servaddr;
  bzero(&servaddr, sizeof(sockaddr_in));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(atoi(argv[2]));
  inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

  connect(clisock, (sockaddr *)&servaddr, sizeof(sockaddr_in));

  // setnonblocking(clisock);
  char buff[1024];
  char temp[128];
  for (int i = 1; i <= 10000; i++) {
    bzero(temp, sizeof(temp));
    bzero(buff, sizeof(buff));
    sprintf(temp, "大猫%d号向您问号", i);
    int len = strlen(temp);
    memcpy(buff, &len, sizeof(len));
    memcpy(buff + sizeof(len), temp, len);
    int num = send(clisock, buff, len + 4, 0);
  }
  std::string msg;
  int count = 0;

  for (int i = 1; i <= 10000; i++) {
    while (inBuff.getMsg(msg)) {

      printf("[CLIENT] : Receive%d %s\n", ++count, msg.data());
      fflush(stdout);
    }
    bzero(buff, sizeof(buff));
    int num = recv(clisock, buff, sizeof(buff), 0);
    if (num > 0)
      inBuff.Append(buff, num);
    else if (num < 0 && errno == EWOULDBLOCK || errno == EAGAIN)
      break;
  }
  // fclose(fp);
}