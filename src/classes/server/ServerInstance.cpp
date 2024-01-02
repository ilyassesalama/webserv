#include "../../../webserv.hpp"

ServerInstance::ServerInstance(s_server &serverInfos): backLog(200) {

    this->initialized = true;
    setListenAdressPort(serverInfos);
    this->duplicated = false;
    this->serverName = serverInfos.server_name;
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
    this->listenPort =  serverInfos.listen.port;
    if(this->listenPort > 65535) {
        Log::e("Port Out Of Range " + String::to_string(serverInfos.listen.port) + " ... ");
        this->initialized = false;
        return;
    }
    this->listenAdress = serverInfos.listen.host;
    // if(String::isIpFormCorrect(this->listenAdress) != true) {
    //     this->initialized = false;
    //     Log::e("Invalid Host Address " + serverInfos.listen.host + " ... ");
    // }
}

void ServerInstance::setListenSocket(int SocketFd) {
    this->listenSocket = SocketFd;
}

std::string ServerInstance::getServerPort() {
    std::stringstream ss;

    ss << this->listenPort;
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

bool ServerInstance::setupServerConfiguration() {
    int opt = 1;

    Log::i("Configuring local network...");
    if(getaddrinfo(getListenAdress().c_str(), getServerPort().c_str(), &this->hint, &this->bindAddress) != 0)
    {
        Log::e("Failed to get address info: " + std::string(gai_strerror(errno)));
        return false;
    }
    Log::i("Created a listening socket on port " + getServerPort());
    this->listenSocket = socket(this->bindAddress->ai_family, this->bindAddress->ai_socktype, this->bindAddress->ai_protocol);
    if(this->listenSocket < 0) {
        Log::e("Failed to create a listening socket due to: " + std::string(strerror(errno)));
        return false;
    }
    setSocketNonBlocking(this->listenSocket);
    if (setsockopt(getListenSocketFd(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        Log::e("Failed to set socket options: " + std::string(strerror(errno)));
        return false;
    }
    Log::i("Binding socket to local address...");
    if(bind(getListenSocketFd(), this->bindAddress->ai_addr, this->bindAddress->ai_addrlen)) {
        Log::e("Failed to bind socket to local address due to: " + std::string(strerror(errno)));
        return false;
    }
    freeaddrinfo(this->bindAddress);
    Log::i("Listening for incoming connections on port " + getServerPort());
    if(listen(getListenSocketFd(),this->backLog) < 0) {
        Log::e("Failed Listening for incoming connections");
        return false;
    }
    AddFdToPollFds(getListenSocketFd());
    return true;
}

std::string extractTheHostFromTheRequest(const std::string& request) {
    size_t findHost = request.find("Host: ");
    if (findHost != std::string::npos) {
        size_t endOfLine = request.find("\r", findHost);
        if (endOfLine != std::string::npos) {
            return request.substr(findHost + 6, endOfLine - (findHost + 6));
        }
    }
    return ""; 
}

int ServerInstance::receiveRequest(int clientFd) {
    std::string receivedRequest;
    int bytesRead;
    char buffer[10000];

    memset(buffer, 0, sizeof(buffer));
    bytesRead = recv(clientFd, buffer, sizeof(buffer), 0);
    if(bytesRead > 0) {
		receivedRequest.append(buffer, bytesRead);
	}
	
    if(bytesRead <= 0) {
        Log::e("Client closed the connection");
        this->dropClient(clientFd);
        return(DROP_CLIENT);
    }
    t_server *server = NULL;
    std::string Host;
    ClientProfile *client = getClientProfile(clientFd);
    if(this->isDuplicated()) {
        //there is multi server
        if(client->serverName != "NONE" && client->isReceiving) {
            server = server = getTheServerConfiguration(client->serverName);
            if(server == NULL)
                server = this->serverInformations;
        }
        else {
            Host = extractTheHostFromTheRequest(receivedRequest);
            std::cout << Host << std::endl;
            client->serverName = Host;
            server = getTheServerConfiguration(Host);
            client->isReceiving = true;
            if(server == NULL)
                server = this->serverInformations;
        }
    }
    else
        server = this->serverInformations;
	client->connectionTime = std::time(0);
	client->request.setServerInformation(server);

	client->requestBuffered += receivedRequest;

	if (client->requestBuffered.find("\r\n\r\n") != std::string::npos) {
		try {
			if (client->isHeaders) {
				client->request.mergeRequestChunks(client->requestBuffered);
				client->isHeaders = false;
			}
			else
				client->request.mergeRequestChunks(receivedRequest);
		} catch (Utils::WebservException &e) {
			Log::e(e.what());
			// return(INVALIDE_REQUEST); // TODO: check if this is needed
		}
	}

    if(client->request.getParsingState().ok) {
        // client->requestStr = client->request.getRequestData();
		client->response.setStatusCode(client->request.getParsingState().statusCode);
        client->response.setPath(client->request.getRequestedResourcePath());
		client->response.setRequest(client->request);
		client->response.setServer(*(this->serverInformations));
        client->response.setRoute(client->request.getRoute());
		client->response.responseBuilder();
        // client->requestStr.clear();
		client->request.nullOutVars();
        client->request.getRequestData().clear();
		client->requestBuffered.clear();
		client->isHeaders = true;
        client->isReceiving = false;
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
    client->connectionTime = std::time(0);
    if(client->response.isUploading()) {
        try {
            client->response.uploadBoundaryFile(); 
            return(999);
        }
        catch(Utils::WebservException &ex) {
            std::cout << ex.what() << std::endl;
        }
    }
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

void ServerInstance::setDuplicated(bool status) {
    this->duplicated = status;
}


void ServerInstance::addDuplicatedServers(t_server *server) {
    this->duplicatedServers.push_back(server);
}

bool ServerInstance::isDuplicated() {
    return(this->duplicated);
}

t_server* ServerInstance::getTheServerConfiguration(std::string host) {
    if (this->getServerName() == host) {
        return this->serverInformations;
    }

    for (std::vector<t_server*>::iterator it = this->duplicatedServers.begin(); it != this->duplicatedServers.end(); ++it) {
        if ((*it)->server_name == host) {
            return *it;
        }
    }

    return NULL;
}