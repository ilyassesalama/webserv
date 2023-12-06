#include"initServer.hpp"



initServer::initServer() {
    std::cout << "init Server Default Constructor Called" << std::endl;
} 

int initServer::create_listening_socket() {
    std::cout << "creating Listenning Socket" << std::endl;
    (*this).socket_listen = socket((*this).bind_address->ai_family, (*this).bind_address->ai_socktype, (*this).bind_address->ai_protocol);
    if((*this).socket_listen < 0)
    {
        //handle error
        std::cout << "Creating Listenning Socket Failed" << std::endl;
        exit(1);
    }
    return(0);
}

int initServer::bind_listenning_socket() {
    std::cout << "Binding Listenning Socket to local address..." << std::endl;
    if (bind((*this).socket_listen, (*this).bind_address->ai_addr, (*this).bind_address->ai_addrlen))
    {
        std::cout << "Binding Faild" << std::endl;
        exit(1);
    }
    freeaddrinfo(bind_address);
    return(0);
}

int initServer::start_listenning() {
    if(listen((*this).socket_listen,100) < 0)
    {
        std::cout << "Listenning failes" << std::endl;
        exit(1);
    }   
    return(0);
}

int initServer::getSocket() {
    return((*this).socket_listen);
}

initServer::initServer(int port) {
    (void)port; // to avoid compilation error
    std::cout << "Configuring local network...\n" << std::endl;
    std::memset(&(*this).hint,0,sizeof((*this).hint));
    (*this).hint.ai_family = AF_INET;
    (*this).hint.ai_socktype = SOCK_STREAM;
    (*this).hint.ai_flags = AI_PASSIVE;
    getaddrinfo(0, "8080", &(*this).hint, &(*this).bind_address);
    create_listening_socket();
    bind_listenning_socket();
    start_listenning();

}

initServer::~initServer() {
    std::cout << "init Server Default Destructor Called" << std::endl;
}