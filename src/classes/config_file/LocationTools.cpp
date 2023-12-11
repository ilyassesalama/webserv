#include "../../../webserv.hpp"

std::string getLocationPath( std::string file, size_t *startIndex ) {
	
	std::string path;

	size_t i;

	for (i = 0; i < file.size(); i++) {
		if (isspace(file[i]) || file[i] == '{') break ;
	}


	if (!isspace(file[i]) && file[i] != '{') throw(Utils::WebservException("Error, there is a syntax error"));
	
	path = file.substr(0, i);

	*startIndex += i;

	return path;
}

size_t getLocationEnd( std::string file ) {

	size_t endPos = file.find("}");

	if (endPos == std::string::npos) throw(Utils::WebservException("Error, there is a syntax error"));

	return endPos;
}

void initRouteBooleans(t_route *route) {
	route->is_allowed_methods = false;
	route->is_redirection = false;
	route->is_root = false;
	route->is_directory_listing = false;
	route->is_is_directory = false;
	route->is_cgi_extension = false;
	route->is_cgi_methods = false;
}

bool isRouteAlreadyExist(t_server server, std::string path) {

	std::vector<t_route>::iterator it;

	for (it = server.routes.begin(); it != server.routes.end(); it++)
		if (it->path == path) return true;
	return false;
}

void skipRoute(std::string file, size_t *startIndex) {
	size_t i = file.find("}");
	if (i == std::string::npos) throw(Utils::WebservException("Error, there is a syntax error"));
	*startIndex += ++i;
}

void checkCGIMethod( std::vector<std::string> values ) {
	bool is_GET = false;
	bool is_POST = false;

	std::vector<std::string>::iterator it;

	for (it = values.begin(); it != values.end(); it++ ) {
		if (*it == "GET" && !is_GET) is_GET = true;
		else if (*it == "POST" && !is_POST) is_POST = true;
		else throw(Utils::WebservException("Error, (cgi_methods): method either not allowed or duplicated"));
	}
}

void checkAllowedMethods(std::vector<std::string>allowed_methods) {

	bool is_GET = false;
	bool is_POST = false;
	bool is_DELETE = false;

	std::vector<std::string>::iterator it;

	for (it = allowed_methods.begin(); it != allowed_methods.end(); it++) {
		if (*it == "GET" && !is_GET) is_GET = true;
		else if (*it == "POST" && !is_POST) is_POST = true;
		else if (*it == "DELETE" && !is_DELETE) is_DELETE = true;
		else throw(Utils::WebservException("Error, (allowed_methods): method either not allowed or duplicated"));
	}
}