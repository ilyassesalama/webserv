#pragma once

#include "../../webserv.hpp"

typedef struct s_listen {
	std::string host;
	int port;
	std::string bindStatus;
	int listenSocket;
} t_listen;

typedef struct s_error_page {
	int error_code;
	std::string error_page;
} t_error_page;

typedef struct s_route {
	std::string path;

	std::string root;
	std::string redirection;
	std::vector<std::string>allowed_methods;
	std::string index;
	std::string upload_path;
	bool directory_listing;
	bool upload;
	bool isCGI;

	// cgi specific
	std::vector<std::string> cgi_extension;

	bool is_allowed_methods;
	bool is_redirection;
	bool is_root;
	bool is_directory_listing;
	bool is_index;
	bool is_cgi_extension;
	bool is_upload;
	bool is_upload_path;
	bool is_isCGI;

} t_route;

typedef struct s_server {
	t_listen listen;
	std::string server_name;
	std::vector<t_error_page>error_pages;
	size_t client_body_size;
	std::string body_size_unit;

	bool is_client_body_size;
	bool is_listen;
	bool is_server_name;

	// Location specific directives

	std::vector<t_route>routes;
} t_server;

class ConfigurationFile {
	public:
		ConfigurationFile( std::string file );

		void handleDirectives( std::string file, t_server server );
		void parseValue( std::string key, std::string value, t_server *server );
		void parseRouteValue( std::string key, std::string value, t_route *route );
		void handleLocation( std::string file, size_t *startIndex, std::string path, t_server *server );

		std::list<t_server>& getConfigFileServers();

		void configFileParsing( void );

		std::list<t_server>ConfigFileServers;
	private:
		std::string file;
};

/* syntax checkers */

void curlyBracesChecker( std::string file, size_t startIndex, size_t endIndex );
void checkBetweenServers( std::string file, int startIndex );
bool checkDuplicateServers(std::list<t_server> &servers, t_server &server);

/* general tools */

size_t serverStartPosition( std::string file );
void getServerCoords(std::string file, size_t *startIndex, size_t *endIndex);
size_t findOpeningBrace( std::string file, size_t endIndex );
void skipSpaces( std::string str, size_t *idx );
int valueCounter( std::string str, int value );
bool isDigits( std::string value );
bool onlySpaces( std::string str );
bool checkErrorCode(int errorCode);
void setDefaultErrors(std::list<t_server> &servers);

/* directives tools */

std::string getDirectiveValue(std::string key, std::string file, size_t *startIndex);
std::string getDirectiveKey( std::string file, size_t *startIndex );
std::string getSingleValue( std::string value, size_t *startPos );
std::string getMultipleValues( std::string value, size_t *startPos );
t_listen parseListen( std::string value );
std::vector<std::string> multipleValuesParser( std::string value );
void parseClientBodySize( size_t *body_size, std::string &body_size_unit, std::string value );
std::vector<t_error_page> parseErrorPage( std::string value );
std::string singleValueParser( std::string value );

/* location tools */

std::string getLocationPath( std::string file, size_t *startIndex );
size_t getLocationEnd( std::string file );
void initRouteBooleans(t_route *route);
bool isRouteAlreadyExist(t_server server, std::string path);
void skipRoute(std::string file, size_t *startIndex);
void checkAllowedMethods(std::vector<std::string>allowed_methods);
std::string getRouteDirectiveValue(std::string key, std::string file, size_t *startIndex);
bool boolParser(std::string &value);