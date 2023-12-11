#include "../../../webserv.hpp"

bool findVal( std::vector<t_error_page>error_pages, int val ) {
	
	std::vector<t_error_page>::iterator it;

	for (it = error_pages.begin(); it != error_pages.end(); it++)
		if (it->error_code == val) return true;
	return false;
}

void ConfigurationFile::parseValue( std::string key, std::string value, t_server *server ) {
	
	if (key == "listen") {
		t_listen listen = parseListen( value );
		server->listen.push_back(listen);
	}
	else if (key == "server_name")
		server->server_names = multipleValuesParser( value );
	else if (key == "client_body_size" && !server->is_client_body_size) {
		parseClientBodySize(&server->client_body_size, server->body_size_unit, value);
		server->is_client_body_size = true;
	} else if (key == "error_page") {

		std::vector<t_error_page>errors = parseErrorPage( value );

		std::vector<t_error_page>::iterator it;
		for (it = errors.begin(); it != errors.end(); it++) {
			if (!findVal(server->error_pages, it->error_code)) server->error_pages.push_back(*it);
		}

	}
	else if (key != "listen" && key != "server_name" && key != "client_body_size" && key != "error_page") {
		throw ("Error, directive not allowed");
	}
}

void ConfigurationFile::parseRouteValue( std::string key, std::string value, t_route *route) {

	if (key == "root" && !route->is_root) {
		route->root = singleValueParser(value);
		route->is_root = true;
	} else if (key == "directory_listing" && !route->is_directory_listing) {
		std::string boolean = singleValueParser(value);
		if (boolean != "true" && boolean != "false") throw ("Error, directory_listing can only be true or false");
		else if (boolean == "true") route->directory_listing = true;
		else if (boolean == "false") route->directory_listing = false;
		route->is_directory_listing = true;
	} else if (key == "is_directory" && !route->is_is_directory) {
		route->is_directory = singleValueParser(value);
		route->is_is_directory = true;
	} else if (key == "cgi_extension" && !route->is_cgi_extension) {
		route->cgi_extension = singleValueParser(value);
		route->is_cgi_extension = true;
	} else if (key == "allowed_methods" && !route->is_allowed_methods) {
		route->allowed_methods = multipleValuesParser(value);
		checkAllowedMethods(route->allowed_methods);
		route->is_allowed_methods = true;
	} else if (key == "cgi_methods" && !route->is_cgi_methods) {
		route->cgi_methods = multipleValuesParser(value);
		checkCGIMethod(route->cgi_methods);
		route->is_cgi_methods = true;
	} else if (key == "redirection" && !route->is_cgi_methods) {
		route->redirection = singleValueParser(value);
		route->is_redirection = true;
	}
	else if (key != "root" && key != "directory_listing" && key != "is_directory" && key != "cgi_extension" && key != "allowed_methods" && key != "cgi_methods") {
		throw ("Error, directive not allowed");
	}
}

void ConfigurationFile::handleLocation( std::string file, size_t *startIndex, std::string path, t_server *server ) {

	std::string directiveKey;
	std::string directiveValue;

	size_t endIndex = getLocationEnd(file);

	size_t i;

	t_route route;

	route.path = path;

	initRouteBooleans(&route);

	for (i = 0; i < endIndex && file[i] != '}'; i++) {
		skipSpaces(file, &i);

		if (file[i] == '}') break ;

		directiveKey = getDirectiveKey( file, &i );

		skipSpaces(file, &i);

		if (directiveKey != "allowed_methods" && directiveKey != "cgi_methods")
			directiveValue = getSingleValue(&file[i], &i);
		else
			directiveValue = getMultipleValues(&file[i], &i);

		parseRouteValue(directiveKey, directiveValue, &route);
	}

	server->routes.push_back(route);

	if (file[i] && file[i] == '}') i++;

	*startIndex += i;
}

void ConfigurationFile::parseDirectives( std::string file, t_server server ) {

	size_t endIndex = serverStartPosition( file );
	if (endIndex == std::string::npos) endIndex = file.size();

	size_t startIndex = findOpeningBrace( file, endIndex );

	std::string directiveKey;
	std::string directiveValue;

	server.is_client_body_size = false;

	for (size_t i = ++startIndex; i < endIndex; i++) {

		skipSpaces(file, &i);

		if (file[i] == '}') break ;

		directiveKey = getDirectiveKey( file, &i );

		skipSpaces(file, &i);

		if (directiveKey != "location") {
				directiveValue = getDirectiveValue(directiveKey, &file[i], &i);

				parseValue(directiveKey, directiveValue, &server);
		}
		else {

			directiveValue = getLocationPath(&file[i], &i);

			if (isRouteAlreadyExist(server, directiveValue)) {
				skipRoute(&file[i], &i);
			} else {
				skipSpaces(file, &i);

				if (file[i] != '{') throw ("Error, there is a syntax error");

				i++;

				handleLocation( &file[i], &i, directiveValue, &server );
			}
		}

	}

	this->ConfigFileServers.push_back(server);

}

void ConfigurationFile::configFileParsing( void ) {

	std::string tmpFile = this->file;

	size_t serverPos = serverStartPosition( tmpFile );

	size_t startIndex;
	size_t endIndex;

	t_server server;

	while ( serverPos != std::string::npos ) {

		getServerCoords(&tmpFile[serverPos + strlen("server")], &startIndex, &endIndex);

		curlyBracesChecker(&tmpFile[serverPos + strlen("server")], startIndex, endIndex);

		parseDirectives(&tmpFile[serverPos + strlen("server")], server);

		checkBetweenServers(&tmpFile[serverPos + strlen("server")], endIndex);

		tmpFile = tmpFile.substr(serverPos + strlen("server"));
		serverPos = serverStartPosition(tmpFile);
	}

}

ConfigurationFile::ConfigurationFile( std::string file ) {
	try {
		this->file = File::getFileContent(file);

		this->configFileParsing();
	}
	catch (const std::exception &e) {
		Log::e("Caught exception due to: " + std::string(e.what()));
		exit(1);
	}
}

std::list<t_server>& ConfigurationFile::getConfigFileServers() {
	return((*this).ConfigFileServers);
}