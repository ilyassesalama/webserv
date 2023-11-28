#include "webserv.hpp"

Network::Network(){
    this->port = 8080; // default port
}

Network::~Network(){
    
}

void Network::startWebServer(void){
    std::cout << "Starting web server on port " << this->port << std::endl;
    // create socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        std::cerr << "Error creating socket" << std::endl;
        exit(EXIT_FAILURE);
    }
    // bind socket to port
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(this->port);
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
        std::cerr << "Error binding socket to port" << std::endl;
        exit(EXIT_FAILURE);
    }
    // listen for connections
    if (listen(server_fd, 3) == -1) {
        std::cerr << "Error listening for connections" << std::endl;
        exit(EXIT_FAILURE);
    }
    // accept connections
    int addrlen = sizeof(address);
    int new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
    if (new_socket == -1) {
        std::cerr << "Error accepting connection" << std::endl;
        exit(EXIT_FAILURE);
    }
    // read request
    char buffer[30000] = {0};
    std::cout << buffer << std::endl;
    // send response
    std::string response = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
    send(new_socket, response.c_str(), response.length(), 0);
    std::cout << "Response sent" << std::endl;
    // close socket
    close(new_socket);
    close(server_fd);
}