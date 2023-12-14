#include "../../../webserv.hpp"

void Response::POSTResponseBuilder() {

	this->setResponseBody();
    this->setHeaders();
    this->setResponseLine();



    this->response = this->responseLine + this->responseHeaders + this->responseBody;

	// std::cout << this->response << std::endl;
}