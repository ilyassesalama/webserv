#include "webserv.hpp"

void printBanner() {
    std::cout << "\033[0;32m"
                 "\n"
              << "██╗    ██╗███████╗██████╗ ███████╗███████╗██████╗ ██╗   ██╗███████╗██████╗ \n"
              << "██║    ██║██╔════╝██╔══██╗██╔════╝██╔════╝██╔══██╗██║   ██║██╔════╝██╔══██╗\n"
              << "██║ █╗ ██║█████╗  ██████╔╝███████╗█████╗  ██████╔╝██║   ██║█████╗  ██████╔╝\n"
              << "██║███╗██║██╔══╝  ██╔══██╗╚════██║██╔══╝  ██╔══██╗╚██╗ ██╔╝██╔══╝  ██╔══██╗\n"
              << "╚███╔███╔╝███████╗██████╔╝███████║███████╗██║  ██║ ╚████╔╝ ███████╗██║  ██║\n"
              << " ╚══╝╚══╝ ╚══════╝╚═════╝ ╚══════╝╚══════╝╚═╝  ╚═╝  ╚═══╝  ╚══════╝╚═╝  ╚═╝\n"
              << "                       Welcome to Web Server!\n"
              << "    ===========================================================\n\n"
              << "\033[0m";
}

// void printServers(ConfigurationFile &cFile) {
// 	// ConfigurationFile cFile(av[1]);

// 	std::list<t_server>::iterator it;

// 	for (it = cFile.ConfigFileServers.begin(); it != cFile.ConfigFileServers.end(); it++) {
		
// 		std::cout << "------------ server -----------" << std::endl;

// 		std::vector<t_listen>::iterator listenIt;
// 		for (listenIt = it->listen.begin(); listenIt != it->listen.end(); listenIt++)
// 			std::cout << "listen host: " << listenIt->host << " listen port: " << listenIt->port << std::endl;

// 		std::vector<std::string>::iterator serverNameIt;
// 		for (serverNameIt = it->server_names.begin(); serverNameIt != it->server_names.end(); serverNameIt++)
// 			std::cout << "server_name: " << *serverNameIt << std::endl;

// 		std::vector<t_error_page>::iterator errIt;
// 		for (errIt = it->error_pages.begin(); errIt != it->error_pages.end(); errIt++)
// 			std::cout << "error_code: " << errIt->error_code << " error_page: " << errIt->error_page << std::endl;

// 		std::cout << "client_body_size: " << it->client_body_size << " client_body_unit: " << it->body_size_unit << std::endl;


// 		std::vector<t_route>::iterator routeIt;
// 		for (routeIt = it->routes.begin(); routeIt != it->routes.end(); routeIt++) {

// 			std::cout << "--------------------" << std::endl;

// 			std::cout << "path: " << routeIt->path << std::endl;

// 			for (serverNameIt = routeIt->allowed_methods.begin(); serverNameIt != routeIt->allowed_methods.end(); serverNameIt++)
// 				std::cout << "allowed_method: " << *serverNameIt << std::endl;

// 			std::cout << "root: " << routeIt->root << std::endl;
// 			std::cout << "redirection: " << routeIt->redirection << std::endl;
// 			std::cout << "is_directory: " << routeIt->is_directory << std::endl;
// 			std::cout << "cgi_extension: " << routeIt->cgi_extension << std::endl;

// 			std::cout << "directory_listing: " << routeIt->directory_listing << std::endl;

// 		}
// 		std::cout << std::endl << std::endl;
// 	}
// }

void startTheParty(ConfigurationFile &obj) {
	Log::i("Server is starting...");
	ConnectionsManager master;
	ServerInstance *server;

    for (std::list<t_server>::iterator it = obj.getConfigFileServers().begin(); it != obj.getConfigFileServers().end(); ++it) {
        ServerInstance s((*it));
		if(s.isInitialized()) {
			server = master.isServerExist(s);
			if(server == NULL) {
				//the server in nor duplicated need to lunch the configuration
				s.setupServerConfiguration();
				master.addServerToTheSet(s);
			}
			else if (server != NULL) {
				//the server is duplicated
				server->setDuplicated(true);
				server->addDuplicatedServers(&(*it));
			}
		}
    }
	master.printMasterFdSet();
	master.printServers();
	if(master.getServerCount() > 0)
		master.socketMonitore();
	else {
		Log::e("No Valid Server To Monitor ...");
		exit(1);
	}

}

int main(int ac, char **av) {
	printBanner();
	Log::setLogFilter(NO_FILTER); // (the default value is NO_FILTER but this is just to ease the debugging process)

	if (ac != 2) {
		Log::e("RUNTIME FAILURE: Configuration file is missing!\n");
		return 1;
	}
	ConfigurationFile cFile(av[1]);
	// printServers(cFile);
	startTheParty(cFile);
	// printServers(cFile);

    return(0);
}