#include"initServer.hpp"


initServer::initServer() {
    (*this).printBanner();
    std::memset(&(*this).hint,0,sizeof((*this).hint)); //we've zeroed out hints
    (*this).hint.ai_family = AF_INET;
    (*this).hint.ai_socktype = SOCK_STREAM;
    (*this).hint.ai_flags = AI_PASSIVE;
    (*this).bind_address = NULL;
    (*this).port = -1;
    (*this).listenSocket = -1;
}

initServer::~initServer() {

}

void initServer::startServer(int port) {
    Log::i("Configuring local network...");
    std::string s_port = std::to_string(port);
    (*this).setPort(port);
    
    getaddrinfo(0, s_port.c_str(), &(*this).hint, &(*this).bind_address);
    setListenSocket();
}

void initServer::setPort(int port) {
    (*this).port = port;
}

int initServer::getPort() {
    return((*this).port);
}

void initServer::setListenSocket() {
    std::string log_msg = "Created a listening socket on port " + std::to_string((*this).port);
    Log::i(log_msg);
    (*this).listenSocket = socket((*this).bind_address->ai_family, (*this).bind_address->ai_socktype, (*this).bind_address->ai_protocol);
    if((*this).listenSocket < 0) {
        Log::e("Failed Creating a listening socket");
        exit(1);
    }
    Log::i("Binding socket to local address...");
    if (bind((*this).listenSocket, (*this).bind_address->ai_addr, (*this).bind_address->ai_addrlen)){
        Log::e("Failed Binding socket to local address...");
        exit(1);
    }
    freeaddrinfo((*this).bind_address);
}

int initServer::getListenSocket() {
    return((*this).listenSocket);
}


#include <iostream>

void initServer::printBanner() {
    std::cout << "\033[1;32m"
                 "\n"
              << "██╗    ██╗███████╗██████╗ ███████╗███████╗██████╗ ██╗   ██╗███████╗██████╗ \n"
              << "██║    ██║██╔════╝██╔══██╗██╔════╝██╔════╝██╔══██╗██║   ██║██╔════╝██╔══██╗\n"
              << "██║ █╗ ██║█████╗  ██████╔╝███████╗█████╗  ██████╔╝██║   ██║█████╗  ██████╔╝\n"
              << "██║███╗██║██╔══╝  ██╔══██╗╚════██║██╔══╝  ██╔══██╗╚██╗ ██╔╝██╔══╝  ██╔══██╗\n"
              << "╚███╔███╔╝███████╗██████╔╝███████║███████╗██║  ██║ ╚████╔╝ ███████╗██║  ██║\n"
              << " ╚══╝╚══╝ ╚══════╝╚═════╝ ╚══════╝╚══════╝╚═╝  ╚═╝  ╚═══╝  ╚══════╝╚═╝  ╚═╝\n"
              << "                       Welcome to Web Server!\n"
              << "    ===========================================================\n\n"
              << "\033[0m";
    Log::d("Server is starting...");
}
