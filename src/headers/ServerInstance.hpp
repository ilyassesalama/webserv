#pragma once

#include "../../webserv.hpp"

class ServerInstance {
    public:
        ServerInstance(s_server &serverInfos);
        
        //SETTERS
        void setListenAdressPort(t_server &serverInfo);
        void setListenSocket(int socketFd);
        void AddFdToPollFds(int clientFd);
        void AddToClientProfiles(ClientProfile &client);

        //GETTERS
        std::string getServerName(); 
        int getListenSocketFd();
        std::string getServerPort();
        std::string getListenAdress();
        std::vector<struct ClientProfile>& getClientProfilesSet();
        std::vector<struct pollfd>& getClientFdSet();
        bool isInitialized();

        //OTHER MEMBERS FUNCTIONS
        void setupServerConfiguration();
        void dropClient(int clientFd);
        bool isClientFdInPollFd(int clientFd);
        int receiveRequest(int clientFd);
        int sendResponse(int clientFd);
        ClientProfile *getClientProfile(int clientFd);
        // bool checkListeningInfos();

    private:
        std::string serverName;
        t_server *serverInformations;
        std::string listenAdress;
        int serverPort;
        int listenSocket;
        struct addrinfo hint;
        struct addrinfo *bindAddress;
        std::vector<struct pollfd> serverPollFd;
        std::vector<struct ClientProfile> clientProfiles;
        const int backLog;
        std::vector<t_listen> listenDirectives;

        bool initialized;
};