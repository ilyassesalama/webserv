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
        void setDuplicated(bool status);
        void addDuplicatedServers(t_server *server);

    private:
        std::string serverName;
        int listenPort;
        std::string listenAdress;
        int listenSocket;
        bool initialized;
        const int backLog;
        bool duplicated;

        t_server *serverInformations;
        struct addrinfo hint;
        struct addrinfo *bindAddress;
        std::vector<struct ClientProfile> clientProfiles;
        std::vector<t_server *>duplicatedServers;
        

        std::vector<struct pollfd> serverPollFd;
};