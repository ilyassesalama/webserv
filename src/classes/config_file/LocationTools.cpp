#include "../../../webserv.hpp"

std::string getLocationPath( std::string file, size_t *startIndex ) {
	
	std::string path;

	size_t i;

	for (i = 0; i < file.size(); i++) {
		if (isspace(file[i]) || file[i] == '{') break ;
	}


	if (!isspace(file[i]) && file[i] != '{') throw(Utils::WebservException("Error, there is no opening brace after location directive"));
	
	path = file.substr(0, i);

	*startIndex += i;

	return path;
}

size_t getLocationEnd( std::string file ) {

	size_t endPos = file.find("}");

	if (endPos == std::string::npos) throw(Utils::WebservException("Error, there is no location closing brace"));

	return endPos;
}

void initRouteBooleans(t_route *route) {
	route->is_allowed_methods = false;
	route->is_redirection = false;
	route->is_root = false;
	route->is_directory_listing = false;
	route->is_index = false;
	route->is_cgi_extension = false;
	route->is_isCGI = false;
	route->is_upload = false;
	route->is_upload_path = false;
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

std::string getRouteDirectiveValue(std::string key, std::string file, size_t *startIndex)
{
	return key != "allowed_methods" && key != "cgi_extension" && key != "redirection" ? getSingleValue(file, startIndex) : getMultipleValues(file, startIndex);
}

bool boolParser(std::string &value) {
	std::string boolean = singleValueParser(value);
	if (boolean != "on" && boolean != "off") throw (Utils::WebservException("Error, directory_listing can only be on or off"));
	else if (boolean == "on") return true;
	return false;
}