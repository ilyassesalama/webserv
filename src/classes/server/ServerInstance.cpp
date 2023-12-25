#include "../../../webserv.hpp"

ServerInstance::ServerInstance(s_server &serverInfos): backLog(200) {
        (*this).initialized = true;
        setListenAdressPort(serverInfos); //check if the array is empty !!!!
        this->serverName = serverInfos.server_names[0];
        this->serverInformations = &serverInfos;
        this->listenSocket = -1;
        this->bindAddress = NULL;
        std::memset(&this->hint,0,sizeof(this->hint));
        this->hint.ai_family = AF_INET;
        this->hint.ai_socktype = SOCK_STREAM;
        this->hint.ai_flags = AI_PASSIVE;
}

std::vector<struct pollfd>& ServerInstance::getClientFdSet() {
    return(this->serverPollFd);
}

std::vector<struct ClientProfile>& ServerInstance::getClientProfilesSet() {
    return(this->clientProfiles);
}


std::string ServerInstance::getServerName() {
    return(this->serverName);
}


void ServerInstance::setListenAdressPort(t_server &serverInfos) {
    this->serverPort =  serverInfos.listen[0].port;
    if(this->serverPort > 65535) {
        Log::e("Port Out Of Range " + String::to_string(serverInfos.listen[0].port) + " ... ");
        (*this).initialized = false;
        return;
    }
    this->listenAdress = serverInfos.listen[0].host;
    if(String::isIpFormCorrect(this->listenAdress) != true) {
        (*this).initialized = false;
        Log::e("Invalid Host Address " + serverInfos.listen[0].host + " ... ");
    }
}

void ServerInstance::setListenSocket(int SocketFd) {
    this->listenSocket = SocketFd;
}

std::string ServerInstance::getServerPort() {
    std::stringstream ss;

    ss << this->serverPort;
    std::string port = ss.str();
    return(port);
}

std::string ServerInstance::getListenAdress() {
    return(this->listenAdress);
}

int ServerInstance::getListenSocketFd() {
    return(this->listenSocket);
}

void ServerInstance::AddFdToPollFds(int clientFd) {
    if (clientFd < 0) {
        Log::e("Invalid Client FD ...");
        return;
    }
    struct pollfd client;
    client.fd = clientFd;
    client.events = POLLIN;
    client.revents = 0;
    this->serverPollFd.push_back(client);
}

void ServerInstance::AddToClientProfiles(ClientProfile &client) {
    this->clientProfiles.push_back(client);
}

void ServerInstance::setupServerConfiguration() {
    int opt = 1;
    
    Log::i("Configuring local network...");
    getaddrinfo(getListenAdress().c_str(), getServerPort().c_str(), &this->hint, &this->bindAddress);
    Log::i("Created a listening socket on port " + getServerPort());
    this->listenSocket = socket(this->bindAddress->ai_family, this->bindAddress->ai_socktype, this->bindAddress->ai_protocol);
    if(this->listenSocket < 0) {
        Log::e("Failed to create a listening socket due to: " + std::string(strerror(errno)));
        exit(1);
    }
    setSocketNonBlocking(this->listenSocket);
    if (setsockopt(getListenSocketFd(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        Log::e("Failed to set socket options: " + std::string(strerror(errno)));
        exit(1);
    }
    Log::i("Binding socket to local address...");
    if(bind(getListenSocketFd(), this->bindAddress->ai_addr, this->bindAddress->ai_addrlen)) {
        Log::e("Failed to bind socket to local address due to: " + std::string(strerror(errno)));
        exit(1);
    }
    freeaddrinfo(this->bindAddress);
    Log::i("Listening for incoming connections on port " + getServerPort());
    if(listen(getListenSocketFd(),this->backLog) < 0) {
        Log::e("Failed Listening for incoming connections");
        exit(1);
    }
    AddFdToPollFds(getListenSocketFd());
}

int ServerInstance::receiveRequest(int clientFd) {
    std::string receivedRequest;
    int bytesRead;
    char buffer[10000];

    memset(buffer, 0, sizeof(buffer));
    bytesRead = recv(clientFd, buffer, sizeof(buffer), 0);
    if(bytesRead > 0) receivedRequest.append(buffer, bytesRead);
    if(bytesRead <= 0) {
        Log::e("Client " + String::to_string(clientFd) + " closed the connection, in other words: " + std::string(strerror(errno)));
        this->dropClient(clientFd);
        return(DROP_CLIENT);
    }

    ClientProfile *client = getClientProfile(clientFd);
	client->connectionTime = std::time(0);
	client->parser.setServerInformation((this->serverInformations));

    try {
        client->parser.mergeRequestChunks(receivedRequest);
    } catch (Utils::WebservException &e) {
        Log::e(e.what());
        return(INVALIDE_REQUEST);
    }
    if(client->parser.getParsingState().ok) {
        client->request = client->parser.getRequestData();
		client->response.setStatusCode(client->parser.getParsingState().statusCode);
        client->response.setPath(client->parser.getRequestedResourcePath());
		client->response.setRequest(client->parser);
		client->response.setServer(*(this->serverInformations));
        client->response.setRoute(client->parser.getRoute());
		client->response.responseBuilder();
        client->request.clear();
		client->parser.nullOutVars();
        client->parser.getRequestData().clear();
        return(FULL_REQUEST_RECEIVED);
    }
    return(INVALIDE_REQUEST);
}

void ServerInstance::dropClient(int clientFd) {
    for(std::vector<struct pollfd>::iterator it = this->serverPollFd.begin(); it != this->serverPollFd.end(); it++) {
        if(it->fd == clientFd) {
            this->serverPollFd.erase(it);
            close(clientFd);
            break;
        }
    }
    for(std::vector<struct ClientProfile>::iterator it = this->clientProfiles.begin(); it != this->clientProfiles.end(); it++) {
        if(it->SocketFD == clientFd) {
            this->clientProfiles.erase(it);
            break;
        }
    }   
}

bool ServerInstance::isClientFdInPollFd(int clientFd) {
    for (std::vector<struct pollfd>::iterator it = this->serverPollFd.begin(); it != this->serverPollFd.end(); it++) {
        if (it->fd == clientFd) {
            return true; 
        }
    }
    return false; 
}

ClientProfile *ServerInstance::getClientProfile(int clientFd) {
    for (std::vector<struct ClientProfile>::iterator it = this->clientProfiles.begin(); it != this->clientProfiles.end(); it++)  {
        if(it->SocketFD == clientFd) {
            return(&(*it));
        }
    }
    return(NULL);
}

int ServerInstance::sendResponse(int clientFd) {
    // static int count = 0;
    // std::cout << clientFd << std::endl;
    ClientProfile *client = getClientProfile(clientFd);
    if(client == NULL) {
        Log::e("client NULL");
    }
    signal(SIGPIPE, SIG_IGN);
    size_t bytesSent = send(clientFd, &client->response.getResponse()[0],client->response.getResponse().size(),0);
    if(bytesSent == 0) {
        Log::e("0 bytes sent");
    }
    // Log::d("Serving clinet ... " + String::to_string(count++));
    if(bytesSent < 0) {
        Log::e("send failed");;
    }
    client->response.setBytesSent(bytesSent);
    client->response.feedDataToTheSender();
    if(client->response.isServing()) {
        return(-1);
    }
    client->connectionTime = std::time(0);
    client->response.clearResponse();

	return 1;
}

bool ServerInstance::isInitialized() {
    return(initialized);
}