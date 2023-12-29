#pragma once

#include "../../webserv.hpp"
class RequestParser;


//prob in the header files UNKOWN TYPE

#include "ClientProfile.hpp"
#include "RequestParser.hpp"
#include "Response.hpp"

struct ClientProfile {
    int SocketFD;
    std::string ipAdress;
    socklen_t address_length;
    struct sockaddr_storage address;
    int port;
    bool isConnected;
    time_t connectionTime;
    int requestCount;
    
    std::string request;
    std::string responseStr;

    RequestParser parser;
    Response response;

	std::string requestBuffered;
	bool isHeaders;
};