#include "../../../webserv.hpp"

void uploadRequest() {

}

void Response::POSTResponseBuilder() {

	this->setResponseBody();
    this->setHeaders();
    this->setResponseLine();

    this->response = this->responseLine + this->responseHeaders + this->responseBody;
}