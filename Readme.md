# 项目描述
&nbsp;&nbsp;&nbsp;&nbsp;该webserver服务器采用主从reactor模式实现，支持http解析和echo功能。项目基于事件循环，每个循环都分配有一个线程。主reactor通过acceptor模块接收用户连接，然后将连接封装成Channel类分配给从reactor，从事件循环使用的是非阻塞的epoll，通过监听socket有无事件读写并回调函数进行处理，实现异步返回结果，这个回调函数是通过工作线程池也就是工作线程来运行的。如果io线程处于空闲状态，可以利用io线程来关闭空闲的连接。

&nbsp;&nbsp;&nbsp;&nbsp;webserver通过模块化进行了严格的任务划分，例如项目中用到了两个线程池，一个是IO线程池，一个是工作线程池（具体的业务逻辑），对于从reactor监听到的读写时间发生后，会先回调对应的IO函数并在IO线程池中进行处理，数据读写完毕后后又会回调工作线程池进行业务处理，业务处理完毕后再调用IO线程池写入响应，这样进一步划分职责项目后期也便于管理以及维护，并通过这种方式实现了类proactor的模式。

# 项目亮点
- 多线程支持：使用线程池处理客户端请求，提高并发处理能力。
- HTTP请求处理：支持解析HTTP请求和生成HTTP响应，处理静态资源请求。
- 日志记录：集成日志模块，记录服务器运行状态和错误信息。
- 数据库连接池：支持与MySQL数据库的连接池管理，提高数据库操作效率。
- 异步I/O：使用epoll实现高效的I/O多路复用，提升服务器性能。
- 模块化设计：代码结构清晰，功能模块化，便于维护和扩展。
- 通过sendfile减少内核数据拷贝。

# 目录结构
- include：头文件目录，包含各模块的头文件。
- src：源代码目录，包含各模块的实现文件。
- resources：静态资源目录，包含HTML、CSS、JavaScript等文件。
- build：构建目录，包含编译生成的文件和中间文件。
- CMakeLists.txt：CMake构建脚本，用于配置和生成构建系统。

# 编译与构建流程
项目在ubuntu20.04下进行构建，需要pthread以及mysqlcppconn连接支持。
```cmake
#安装mysql连接支持
sudo apt install libmysqlcppconn-dev
#数据库导入
mysql -u root -p root < ./sqldata.sql
#编译构建项目
cmake -S . -B build
cmake --build build
#运行项目
./JammyWebserver/build/src/http/http_server ip port
```
浏览器输入http://ip:port进行访问。

# 感谢
项目借鉴了[TinyWebServer](https://github.com/qinguoyi/TinyWebServer)
