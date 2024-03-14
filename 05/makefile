
epoll_server : epoll_server.cpp InetAddr.cpp Socket.cpp Epoll.cpp
	g++ -g $^ -o $@ 

epoll_client : epoll_client.cpp InetAddr.cpp Socket.cpp Epoll.cpp
	g++ -g $^ -o $@ 

all : epoll_server epoll_client

clean :
	rm epoll_client epoll_server

.PHONY : clean