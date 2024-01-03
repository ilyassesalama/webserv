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

void startTheParty(ConfigurationFile &obj) {
	Log::i("Server is starting...");
	ConnectionsManager master;
	ServerInstance *server;

    for (std::list<t_server>::iterator it = obj.getConfigFileServers().begin(); it != obj.getConfigFileServers().end(); ++it) {
        ServerInstance s((*it));
		if(s.isInitialized()) {
			server = master.isServerExist(s);
			if(server == NULL) {
				// the server in nor duplicated need to lunch the configuration
				if(s.setupServerConfiguration() == true)
					master.addServerToTheSet(s);
			}
			else if (server != NULL) {
				// the server is duplicated
				server->setDuplicated(true);
				server->addDuplicatedServers(&(*it));
			}
		}
    }
	if(master.getServerCount() > 0)
		master.socketMonitor();
	else {
		Log::e("No valid server to monitor");
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
	startTheParty(cFile);
    return(0);
}