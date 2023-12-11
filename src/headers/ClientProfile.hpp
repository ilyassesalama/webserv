#ifndef CLIENTPROFILE_HPP
#define CLIENTPROFILE_HPP

#include"libs.hpp"

struct ClientProfile {
    int SocketFD;
    std::string ipAdress;
    socklen_t address_length;
    struct sockaddr_storage address;
    int port;
    bool isConnected;
    time_t connectionsTime;
    int requestCount;
    std::string request;
    std::string response;
};





#endif