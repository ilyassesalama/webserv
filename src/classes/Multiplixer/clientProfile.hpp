#ifndef CLIENTPROFILE_HPP
#define CLIENTPROFILE_HPP

#include"libs.hpp"

struct clientProfile {
    int SocketFD;
    std::string ipAdress;
    socklen_t address_length;
    struct sockaddr_storage address;
    int port;
    bool isConnected;
    time_t connectionsTime;
    int requestCount;
    char read_bits[1024];
    
    clientProfile(int portNumber);
    void incRequestCount();
};





#endif