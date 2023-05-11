#ifndef __SOCKET_HPP__
#define __SOCKET_HPP__

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <string>
#include <vector>

int create_server_socket(const char *port);
int create_client_socket(const char *host, const char *port);
int server_accept(int socket_fd, std::string &addr);
int get_port(int socket);
int send_data(int socket, const void *data, int data_len);
int recv_data(int socket, void *data, int data_len);

#endif