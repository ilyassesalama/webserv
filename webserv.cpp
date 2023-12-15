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
    Log::i("Server is starting...");
}

void printServers(ConfigurationFile &cFile) {
	// ConfigurationFile cFile(argv[1]);

	std::list<t_server>::iterator it;

	for (it = cFile.ConfigFileServers.begin(); it != cFile.ConfigFileServers.end(); it++) {
		
		std::cout << "------------ server -----------" << std::endl;

		std::vector<t_listen>::iterator listenIt;
		for (listenIt = it->listen.begin(); listenIt != it->listen.end(); listenIt++)
			std::cout << "listen host: " << listenIt->host << " listen port: " << listenIt->port << std::endl;

		std::vector<std::string>::iterator serverNameIt;
		for (serverNameIt = it->server_names.begin(); serverNameIt != it->server_names.end(); serverNameIt++)
			std::cout << "server_name: " << *serverNameIt << std::endl;

		std::vector<t_error_page>::iterator errIt;
		for (errIt = it->error_pages.begin(); errIt != it->error_pages.end(); errIt++)
			std::cout << "error_code: " << errIt->error_code << " error_page: " << errIt->error_page << std::endl;

		std::cout << "client_body_size: " << it->client_body_size << " client_body_unit: " << it->body_size_unit << std::endl;


		std::vector<t_route>::iterator routeIt;
		for (routeIt = it->routes.begin(); routeIt != it->routes.end(); routeIt++) {

			std::cout << "--------------------" << std::endl;

			std::cout << "path: " << routeIt->path << std::endl;

			for (serverNameIt = routeIt->allowed_methods.begin(); serverNameIt != routeIt->allowed_methods.end(); serverNameIt++)
				std::cout << "allowed_method: " << *serverNameIt << std::endl;

			for (serverNameIt = routeIt->cgi_methods.begin(); serverNameIt != routeIt->cgi_methods.end(); serverNameIt++)
				std::cout << "cgi_method: " << *serverNameIt << std::endl;

			std::cout << "root: " << routeIt->root << std::endl;
			std::cout << "redirection: " << routeIt->redirection << std::endl;
			std::cout << "is_directory: " << routeIt->is_directory << std::endl;
			std::cout << "cgi_extension: " << routeIt->cgi_extension << std::endl;

			std::cout << "directory_listing: " << routeIt->directory_listing << std::endl;

		}
		std::cout << std::endl << std::endl;
	}
}

// void parsingTest(){
//     RequestParser requestParser;
//     std::map<std::string, std::string> requestLine;
//     std::map<std::string, std::string> headers;
//     std::string body;


//     std::string httpRequest = "GET /example/path/resource?param1=value1&param3=value2&param2=value2&param2=value2 HTTP/1.1\r\n"
//                               "Host: www.1337.ma\r\n"
//                               "User-Agent: Mozilla/5.0\r\n"
//                               "Accept: text/html,*/*\r\n"
//                               "Accept-Language: en-us\r\n"
//                               "Accept-Charset: ISO-8859-1,utf-8\r\n"
//                               "Connection: keep-alive\r\n"
//                               "\r\n"
//                               "body of the request";

//     requestParser.initRequestParser(httpRequest);

//     requestLine = requestParser.getRequestLine();
//     headers = requestParser.getHeaders();
//     body = requestParser.getBody();

//     std::cout << "-------------- request line ----------------" << std::endl;

//     std::cout << "method: " << requestLine["method"] << std::endl;
//     std::cout << "path: " << requestLine["path"] << std::endl;
//     std::cout << "httpVersion: " << requestLine["httpVersion"] << std::endl;

//     std::cout << "-------------- request headers ----------------" << std::endl;
//     std::map<std::string, std::string>::iterator it;
//     for (it = headers.begin(); it != headers.end(); it++){
//         std::cout << it->first << ": " << it->second << std::endl;
//     }

//     std::cout << "-------------- request params ----------------" << std::endl;
//     std::map<std::string, std::string> params = requestParser.getParams();
//     for (it = params.begin(); it != params.end(); it++){
//         std::cout << it->first << ": " << it->second << std::endl;
//     }

//     std::cout << "--------------- body ---------------" << std::endl;

//     std::cout << body << std::endl;

// }

void startTheParty(ConfigurationFile &obj) {
	printBanner();
	ConnectionsManager master;

    for (std::list<t_server>::iterator it = obj.getConfigFileServers().begin(); it != obj.getConfigFileServers().end(); ++it) {
        ServerInstance s((*it));
		s.setupServerConfiguration();
		master.addServerToTheSet(s);
    }
	master.socketMonitore();

}



int main(int argc, char *argv[]) {

	if (argc != 2) return 1;

	ConfigurationFile cFile(argv[1]);
	// printServers(cFile);
	startTheParty(cFile);
	// printServers(cFile);

    return(0);
}