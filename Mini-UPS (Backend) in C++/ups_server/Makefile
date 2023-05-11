CC=g++
CCFLAGS= -pedantic -Wall -Wextra -Werror -ggdb3
EXTRAFLAGS= -lpqxx -lpq `pkg-config --cflags --libs protobuf` -pthread

all: server

server: main.cpp database_func.hpp pkg.hpp gpb_server.hpp socket.hpp truck.hpp myAUcommands.hpp myUResponses.hpp
	$(CC) $(CFLAGS) -o server main.cpp database_func.cpp pkg.hpp gpb_server.cpp socket.cpp truck.hpp myAUcommands.cpp myUResponses.cpp ./proto_files/world_ups.pb.cc ./proto_files/ups_amazon.pb.cc $(EXTRAFLAGS)

.PHONY:
	clean
clean:
	rm -rf *.o server
