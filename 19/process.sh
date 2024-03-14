#!/bin/bash

# 循环100次，每次启动一个后台进程
for i in $(seq 1 2000); do
        log_file="./log/log${i}.txt"
        ./epoll_client 121.37.41.140 8888 > "$log_file"&
done