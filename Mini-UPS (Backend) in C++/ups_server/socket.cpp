#include "socket.hpp"

using namespace std;

int create_server_socket(const char *port) {
    // start the server
    int status;
    int socket_fd;
    struct addrinfo hint;
    struct addrinfo *host_info_list;
    const char *hostname = NULL;
    memset(&hint, 0, sizeof(hint));
    hint.ai_family = AF_UNSPEC;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_flags = AI_PASSIVE;

    status = getaddrinfo(hostname, port, &hint, &host_info_list);
    if (status != 0) {
        std::cerr << "Error: cannot get address info for host" << std::endl;
        std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
        exit(EXIT_FAILURE);
    }
    if (strcmp(port, "") == 0) {
        // or find a random port
        struct sockaddr_in *addr_in = (struct sockaddr_in *)(host_info_list->ai_addr);
        addr_in->sin_port = 0;
    }
    socket_fd = socket(host_info_list->ai_family, host_info_list->ai_socktype, host_info_list->ai_protocol);
    if (socket_fd == -1) {
        std::cerr << "Error: cannot create socket" << std::endl;
        std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
        exit(EXIT_FAILURE);
    }
    int yes = 1;
    status = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    /* added my myself*/
    if (status == -1) {
        std::cerr << "setsockopt" << std::endl;
        exit(EXIT_FAILURE);
    }
    status = bind(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);  // binding ip and port
    if (status == -1) {
        std::cerr << "Error: cannot bind socket" << std::endl;
        std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
        freeaddrinfo(host_info_list);
        exit(EXIT_FAILURE);
    }
    status = listen(socket_fd, 100);
    if (status == -1) {
        std::cerr << "Error: cannot listen on socket" << std::endl;
        std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
        freeaddrinfo(host_info_list);
        exit(EXIT_FAILURE);
    }
    int client_fd = accept(socket_fd, NULL, NULL);
    freeaddrinfo(host_info_list);
    return client_fd;
}

int create_client_socket(const char *host, const char *port) {
    // start the client
    int status;
    int socket_fd;
    struct addrinfo hint;
    struct addrinfo *host_info_list;
    memset(&hint, 0, sizeof(hint));
    hint.ai_family = AF_UNSPEC;
    hint.ai_socktype = SOCK_STREAM;
    status = getaddrinfo(host, port, &hint, &host_info_list);
    if (status != 0) {
        std::cerr << "Error: cannot get address info for host" << std::endl;
        std::cerr << "  (" << host << "," << port << ")" << std::endl;
        exit(EXIT_FAILURE);
    }
    socket_fd = socket(host_info_list->ai_family, host_info_list->ai_socktype, host_info_list->ai_protocol);
    if (socket_fd == -1) {
        std::cerr << "Error: cannot create socket" << std::endl;
        std::cerr << "  (" << host << "," << port << ")" << std::endl;
        exit(EXIT_FAILURE);
    }
    status = connect(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    if (status == -1) {
        std::cerr << "Error: cannot connect to socket" << std::endl;
        std::cerr << "  (" << host << "," << port << ")" << std::endl;
        freeaddrinfo(host_info_list);
        exit(EXIT_FAILURE);
    }
    freeaddrinfo(host_info_list);
    return socket_fd;
}

int server_accept(int socket_fd, string &addr) {
    // server accept a connection
    int client_connection_fd;
    struct sockaddr_storage socket_addr;
    socklen_t socket_addr_len = sizeof(socket_addr);
    client_connection_fd = accept(socket_fd, (struct sockaddr *)&socket_addr, &socket_addr_len);
    if (client_connection_fd == -1) {
        std::cerr << "Error: cannot accept connection on socket" << std::endl;
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in *newaddr = (struct sockaddr_in *)&socket_addr;
    addr = inet_ntoa(newaddr->sin_addr);
    return client_connection_fd;
}

int get_port(int socket) {
    // get the port number
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    int status = getsockname(socket, (struct sockaddr *)&addr, &addr_len);
    if (status == -1) {
        std::cerr << "Error: cannot get socket name" << std::endl;
        exit(EXIT_FAILURE);
    }
    return ntohs(addr.sin_port);
}

int send_data(int socket, const void *data, int data_len) {
    int have_sent = 0;
    int bytes_left = data_len;
    int one_time_send = 0;
    while (have_sent < static_cast<int>(data_len)) {
        one_time_send = send(socket, (char *)data + have_sent, bytes_left, 0);
        if (one_time_send == -1) {
            break;
        }
        have_sent += one_time_send;
        bytes_left -= one_time_send;
    }
    return one_time_send == -1 ? -1 : have_sent;  // return -1 on failure, 0 on success
}

int recv_data(int socket, void *data, int data_len) {
    int have_recv = 0;
    int bytes_left = data_len;
    int one_time_recv = 0;
    while (have_recv < static_cast<int>(data_len)) {
        one_time_recv = recv(socket, (char *)data + have_recv, bytes_left, 0);
        if (one_time_recv == -1) {
            break;
        }
        have_recv += one_time_recv;
        bytes_left -= one_time_recv;
    }
    return one_time_recv == -1 ? -1 : have_recv;  // return -1 on failure, 0 on success
}