#!/bin/bash
rm -rf ./build/log
mkdir -p ./build/log
# 循环100次，每次启动一个后台客户端进程
for i in $(seq 1 1000); do
        log_file="./build/log/log${i}.txt"
        ./build/src/test_client/test_client 127.0.0.1 8888 > "$log_file"&
done