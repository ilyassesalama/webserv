#include "../../../webserv.hpp"

void Response::GETResponseBuilder() {
    this->setResponseBody();
    this->setHeaders();
    this->setResponseLine();
    addDataToResponse(this->responseLine);
    addDataToResponse(this->responseHeaders);
    addDataToResponse(this->responseBody);
    setServingState(true);
}