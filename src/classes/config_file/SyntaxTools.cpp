#include "../../../webserv.hpp"

void curlyBracesChecker( std::string file, size_t startIndex, size_t endIndex ) {

	size_t i = startIndex;
	int countOpeners = 0;
	bool serverClosed = false;


	if (file[i] != '{') throw(Utils::WebservException("Error, there is a syntax error"));

	for (; i < endIndex; i++) {
		if (file[i] == '{') {
			if (serverClosed) throw(Utils::WebservException("Error, there is a syntax error"));
			countOpeners++;
		}
		else if (file[i] == '}' && countOpeners == 1) {
			serverClosed = true;
			countOpeners--;
		}
		else if (file[i] == '}' && countOpeners > 1) countOpeners--;
		else if (file[i] == '}' && countOpeners == 0) throw(Utils::WebservException("Error, there is a syntax error "));
		if ((file[i] == '{' || file[i] == '}') && i + 1 < endIndex && file[i + 1] == ';') throw(Utils::WebservException("Error, there is a syntax error"));
	}

	if (countOpeners != 0 || !serverClosed) throw(Utils::WebservException("Error, there is a syntax error"));
}

void checkBetweenServers( std::string file, int endIndex ) {

	// Here I searched for the next server pos if not found set to file size
	// and I looked if anything other than space exist if anything other than space exist obviously that is an error

	for (size_t i = --endIndex; file[i] != '}'; i--) {
		if (!isspace(file[i])) throw(Utils::WebservException("Error, there is a syntax error"));
	}

}

bool checkDuplicateServers(std::list<t_server> &servers, t_server &server) {

	std::list<t_server>::iterator it;

	for (it = servers.begin(); it != servers.end(); it++) {
		if (it->listen.host == server.listen.host && it->listen.port == server.listen.port && it->server_name == server.server_name)
			return false;
	}
	return true;

}