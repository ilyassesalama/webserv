#include "../../../webserv.hpp"

void Response::GETResponseBuilder() {

    this->setResponseBody();
    this->setHeaders();
    this->setResponseLine();

    this->response = this->responseLine + this->responseHeaders + this->responseBody;
}