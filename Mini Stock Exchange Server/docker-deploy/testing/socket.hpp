#include <arpa/inet.h>  //used for inet_ntoa()
#include <netdb.h>      //used for getIP, getPort instructions
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>  //used for close()

#include <cstdlib>
#include <cstring>   //used for memset(), strcpy()
#include <iostream>  //used for std::cerr
#include <string>
#include <vector>

int send_data(int socket_fd, void *data, size_t data_len) {
    int have_sent = 0;
    int bytes_left = data_len;
    int one_time_send = 0;
    while (have_sent < static_cast<int>(data_len)) {
        one_time_send = send(socket_fd, (char *)data + have_sent, bytes_left, MSG_WAITALL);
        if (one_time_send == -1) {
            break;
        }
        have_sent += one_time_send;
        bytes_left -= one_time_send;
    }
    return one_time_send == -1 ? -1 : have_sent;  // return -1 on failure, 0 on success
}

int recv_data(int socket_fd, void *data, size_t data_len) {
    int have_recv = 0;
    int bytes_left = data_len;
    int one_time_recv = 0;
    while (have_recv < static_cast<int>(data_len)) {
        one_time_recv = recv(socket_fd, (char *)data + have_recv, bytes_left, MSG_WAITALL);
        if (one_time_recv == -1) {
            break;
        }
        have_recv += one_time_recv;
        bytes_left -= one_time_recv;
    }
    return one_time_recv == -1 ? -1 : have_recv;  // return -1 on failure, 0 on success
}

// this function is modified from tcp_example/server.cpp
int create_server(const char *hostname, const char *port) {
    int status;
    int socket_fd;
    struct addrinfo host_info;
    struct addrinfo *host_info_list;

    memset(&host_info, 0, sizeof(host_info));

    host_info.ai_family = AF_UNSPEC;
    host_info.ai_socktype = SOCK_STREAM;
    host_info.ai_flags = AI_PASSIVE;

    // get address information for host
    status = getaddrinfo(hostname, port, &host_info, &host_info_list);
    if (status != 0) {
        std::cerr << "Error: cannot get address info for host" << std::endl;
        std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
        exit(EXIT_FAILURE);
    }

    // create socket
    socket_fd = socket(host_info_list->ai_family, host_info_list->ai_socktype, host_info_list->ai_protocol);
    if (socket_fd == -1) {
        std::cerr << "Error: cannot create socket" << std::endl;
        std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
        exit(EXIT_FAILURE);
    }

    // bind socket
    int yes = 1;
    status = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    status = bind(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    if (status == -1) {
        std::cerr << "Error: cannot bind socket" << std::endl;
        std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
        exit(EXIT_FAILURE);
    }

    // listen on socket
    status = listen(socket_fd, 100);
    if (status == -1) {
        std::cerr << "Error: cannot listen on socket" << std::endl;
        std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(host_info_list);
    return socket_fd;
}

// this function is modified from tcp_example/client.cpp
int create_client(const char *hostname, const char *port) {
    int status;
    int socket_fd;
    struct addrinfo host_info;
    struct addrinfo *host_info_list;

    memset(&host_info, 0, sizeof(host_info));
    host_info.ai_family = AF_UNSPEC;
    host_info.ai_socktype = SOCK_STREAM;

    // get address information for host
    status = getaddrinfo(hostname, port, &host_info, &host_info_list);
    if (status != 0) {
        std::cerr << "Error: cannot get address info for host" << std::endl;
        std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
        exit(EXIT_FAILURE);
    }

    // create socket
    socket_fd = socket(host_info_list->ai_family, host_info_list->ai_socktype, host_info_list->ai_protocol);
    if (socket_fd == -1) {
        std::cerr << "Error: cannot create socket" << std::endl;
        std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
        exit(EXIT_FAILURE);
    }

    // connect to socket
    status = connect(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    if (status == -1) {
        std::cerr << "Error: cannot connect to socket" << std::endl;
        std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(host_info_list);
    return socket_fd;
}