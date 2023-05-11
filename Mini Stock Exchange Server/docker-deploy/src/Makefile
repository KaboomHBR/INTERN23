CC=g++
CCFLAGS= -pedantic -Wall -Werror -ggdb3
EXTRAFLAGS=-lpqxx -lpq -pthread -lxerces-c

all: server

server: main.cpp socket.hpp order.hpp handle_xml.hpp database_func.hpp tinyxml2.hpp server.hpp
	$(CC) $(CFLAGS)  -o server main.cpp socket.hpp order.hpp handle_xml.hpp handle_xml.cpp database_func.hpp database_func.cpp tinyxml2.hpp tinyxml2.cpp server.cpp $(EXTRAFLAGS)

.PHONY:
	clean
clean:
	rm -rf *.o server
