#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <stdexcept>
#include <thread>

#include "handle_xml.hpp"
#include "server.hpp"
#include "socket.hpp"

using namespace std;

void handleClient(int client_connection_fd) {
    // todo: robust????
    //  establish connection
    connection* C;
    try {
        C = new connection("dbname=matching_server user=postgres password=passw0rd");
        if (C->is_open()) {
            std::cout << "Opened database successfully: " << C->dbname() << std::endl;
        } else {
            std::cerr << "Failed to open database" << std::endl;
            exit(EXIT_FAILURE);
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return;
    }

    char request[65536] = {0};
    int count = 0;
    // initialize variable "start" with the current time
    time_t start = time(NULL);
    while (true) {
        memset(request, 0, sizeof(request));
        // reveive request from client
        int requestLength = recv(client_connection_fd, request, sizeof(request), 0);
        cout << "requestLength is: " << requestLength << endl;
        // case 1: client connection has been closed gracefully
        if (requestLength == 0) {
            break;
        }
        // case 2: error occurs
        if (requestLength == -1) {
            std::cerr << "Failed to receive request from client" << std::endl;
            return;
        }
        // case 3: request is received succesfully
        std::string Request, request_copy, response;
        Request.assign(request, requestLength);
        request_copy = Request;
        while (request_copy.length() > 0) {
            // extract the first line of request (an integer)
            size_t firstLineIndex = request_copy.find('\n');
            std::string firstLine = request_copy.substr(0, firstLineIndex);
            int number = std::stoi(firstLine);  // note that number is the length of xml (without '\0')
            cout << "number is: " << number << endl;
            // extract the rest of request (the xml part, or the process string)
            std::string xml = request_copy.substr(firstLineIndex + 1, number);
            cout << "xml sliced successful" << endl;
            cout << "xml is: " << xml << endl;
            int handle = handle_request(C, xml, response);
            if (handle == 1) {
                std::cout << response << std::endl;
                return;
            }
            // ParseRequets(C, xml, response);
            // print response
            std::cout << "response generated in server---" << endl
                      << response << std::endl;
            // send the reponse to client
            int sendStatus = send(client_connection_fd, response.c_str(), response.length(), 0);
            std::cout << "send response length is: " << sendStatus << std::endl;
            // go on the process the next request
            count++;
            std::cout << "server counts: " << count << std::endl;
            size_t nextIndex = (firstLineIndex + 1) + number;
            request_copy = request_copy.substr(nextIndex, std::string::npos);
        }
        time_t end = time(NULL);
        std::cout << "runtime is: " << (end - start) << std::endl;
    }
    C->disconnect();
    delete C;
}

int main(int argc, char** argv) {
    // establish connection
    connection* C;
    try {
        C = new connection("dbname=matching_server user=postgres password=passw0rd");
        if (C->is_open()) {
            string str = "set transaction isolation level serializable;";  // todo: not sure
            sql_exec(C, str);                                              // todo: not sure
            std::cout << "Opened database successfully: " << C->dbname() << std::endl;
        } else {
            // std::cerr << "Failed to open database" << std::endl;
            // return EXIT_FAILURE;
            throw std::exception();
        }
    } catch (const std::exception& e) {
        // std::cerr << e.what() << std::endl;
        std ::cout << "Failed to open database" << std::endl;
        return EXIT_FAILURE;
    }

    drop_tables(C);
    create_table_SYMBOL(C);
    create_table_ACCOUNT(C);
    create_table_TRANSACTION(C);
    create_table_EXECUTED(C);
    C->disconnect();
    delete C;

    // initialize a server to establish connections with all clients
    const char* hostname = NULL;
    const char* port = "12345";
    int socket_fd = create_server(hostname, port);
    while (true) {
        struct sockaddr_storage socket_addr;
        socklen_t socket_addr_len = sizeof(socket_addr);
        int client_connection_fd = accept(socket_fd, (struct sockaddr*)&socket_addr, &socket_addr_len);
        if (client_connection_fd == -1) {
            std::cerr << "Error: cannot accept connection on socket" << std::endl;
            return EXIT_FAILURE;
        }
        struct sockaddr_in* addr = (struct sockaddr_in*)&socket_addr;
        std::string client_IP = inet_ntoa(addr->sin_addr);
        std::cout << "Successfully connected with the client from " << client_IP << std::endl;
        // handleClient(client_connection_fd);  // todo: no multithread
        // cout << "handleClient!!!" << endl;   // todo: no multithread

        // create a new thread of execution
        std::thread newthread(handleClient, client_connection_fd);
        // detach the child thread from its parent thread
        newthread.detach();
    }
    close(socket_fd);
    return EXIT_SUCCESS;
}
