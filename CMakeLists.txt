CMAKE_MINIMUM_REQUIRED(VERSION 3.10)
project(jammy_webserver VERSION 1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED true)

include_directories(include build)
# configure_file("config.h.in" "config.h")


add_executable(jammy_server 
"${PROJECT_SOURCE_DIR}/src/jammy_server.cpp" "${PROJECT_SOURCE_DIR}/src/EchoServer.cpp" "${PROJECT_SOURCE_DIR}/src/TcpServer.cpp" 
"${PROJECT_SOURCE_DIR}/src/Acceptor.cpp" "${PROJECT_SOURCE_DIR}/src/Connection.cpp" "${PROJECT_SOURCE_DIR}/src/Channel.cpp"
"${PROJECT_SOURCE_DIR}/src/ThreadPool.cpp" "${PROJECT_SOURCE_DIR}/src/EpollLoop.cpp" "${PROJECT_SOURCE_DIR}/src/Epoll.cpp" 
"${PROJECT_SOURCE_DIR}/src/Socket.cpp" "${PROJECT_SOURCE_DIR}/src/InetAddr.cpp" "${PROJECT_SOURCE_DIR}/src/TimeStamp.cpp"
"${PROJECT_SOURCE_DIR}/src/Buffer.cpp"
)
add_executable(jammy_client "${PROJECT_SOURCE_DIR}/src/jammy_client.cpp" "${PROJECT_SOURCE_DIR}/src/Buffer.cpp")

target_link_libraries(jammy_server pthread)
