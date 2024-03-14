
epoll_server : epoll_server.cpp
	g++ -g $^ -o $@ 

epoll_client : epoll_client.cpp Buffer.cpp
	g++ -g $^ -o $@ 

all : epoll_server epoll_client

clean :
	rm epoll_client epoll_server

.PHONY : clean