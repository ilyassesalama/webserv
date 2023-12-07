#ifndef CLIENTPROFILE_HPP
#define CLIENTPROFILE_HPP

#include"libs.hpp"

struct clientProfile {
    std::string SocketType;
    int SocketFD;
    std::string ipAdress;
    socklen_t address_length;
    struct sockaddr_storage address;
    int port;
    bool isConnected;
    time_t connectionsTime;
    time_t lastRequestTime;
    int requestCount;

    clientProfile(int portNumber);
};





#endif