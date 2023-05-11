CC=g++
CFLAGS=-std=c++11
EXTRAFLAGS=-lpqxx -lpq -lpthread -lxerces-c

all: client

client: client.cpp client.hpp socket.hpp
	$(CC) $(CFLAGS)  -o client client.cpp client.hpp socket.hpp $(EXTRAFLAGS)


.PHONY:
	clean
clean:
	rm -rf *.o client