#ifndef __SERVER_HPP__
#define __SERVER_HPP__

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

#define PORT "12345"

using namespace std;

class Server {
   public:
    int status;
    int socket_fd;
    struct addrinfo host_info;
    struct addrinfo *host_info_list;
    const char *hostname;
    const char *port;
    Server(const char *port) : status(0), socket_fd(0), host_info_list(NULL), hostname(NULL), port(port) {
        init_serverSocket();
    }
    ~Server() {
        if (socket_fd != 0) {
            close(socket_fd);
        }
        if (host_info_list != NULL) {
            free(host_info_list);
        }
    }
    void init_serverSocket();
    int accept_connection(string *ip_addr);
    string getHostPortInfo();
};

class Parameter {
   public:
    int client_fd;
    Parameter(int fd) : client_fd(fd) {}
};

#endif