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
    logs("Configuring local network...");
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
    logs(log_msg);
    (*this).listenSocket = socket((*this).bind_address->ai_family, (*this).bind_address->ai_socktype, (*this).bind_address->ai_protocol);
    if((*this).listenSocket < 0) {
        logs("Failed Creating a listening socket");
        exit(1);
    }
    logs("Binding socket to local address...");
    if (bind((*this).listenSocket, (*this).bind_address->ai_addr, (*this).bind_address->ai_addrlen)){
        logs("Failed Binding socket to local address...");
        exit(1);
    }
    freeaddrinfo((*this).bind_address);
}

int initServer::getListenSocket() {
    return((*this).listenSocket);
}


void initServer::printBanner() {
    std::cout << "\033[1;32m" // ANSI escape code for red text
              << "  .----------------.  .----------------.  .----------------.  .----------------.  .----------------.  .----------------.  .----------------.  .----------------.  .----------------.\n"
              << " | .--------------. || .--------------. || .--------------. || .--------------. || .--------------. || .--------------. || .--------------. || .--------------. || .--------------. |\n"
              << " | | _____  _____ | || |  _________   | || |   ______     | || |    _______   | || |  _________   | || |  _______     | || | ____   ____  | || |  _________   | || |  _______     | |\n"
              << " | ||_   _||_   _|| || | |_   ___  |  | || |  |_   _ \\    | || |   /  ___  |  | || | |_   ___  |  | || | |_   __ \\    | || ||_  _| |_  _| | || | |_   ___  |  | || | |_   __ \\    | |\n"
              << " | |  | | /\\ | |  | || |   | |_  \\_|  | || |    | |_) |   | || |  |  (__ \\_|  | || |   | |_  \\_|  | || |   | |__) |   | || |  \\ \\   / /   | || |   | |_  \\_|  | || |   | |__) |   | |\n"
              << " | |  | |/  \\| |  | || |   |  _|  _   | || |    |  __'.   | || |   '.___`-.   | || |   |  _|  _   | || |   |  __ /    | || |   \\ \\ / /    | || |   |  _|  _   | || |   |  __ /    | |\n"
              << " | |  |   /\\   |  | || |  _| |___/ |  | || |   _| |__) |  | || |  |`\\____) |  | || |  _| |___/ |  | || |  _| |  \\ \\_  | || |    \\ ' /     | || |  _| |___/ |  | || |  _| |  \\ \\_  | |\n"
              << " | |  |__/  \\__|  | || | |_________|  | || |  |_______/   | || |  |_______.'  | || | |_________|  | || | |____| |___| | || |     \\_/      | || | |_________|  | || | |____| |___| | |\n"
              << " | |              | || |              | || |              | || |              | || |              | || |              | || |              | || |              | || |              | |\n"
              << " | '--------------' || '--------------' || '--------------' || '--------------' || '--------------' || '--------------' || '--------------' || '--------------' || '--------------' |\n"
              << "  '----------------'  '----------------'  '----------------'  '----------------'  '----------------'  '----------------'  '----------------'  '----------------'  '----------------'\n"
              << "\033[0m"; // ANSI escape code to reset text color
    std::cout << "                                                                             Welcome to Web Server!\n";
    std::cout << "                                                           ===========================================================\n";
    logs("Server is starting...");
}