#include "../../../webserv.hpp"

void Response::DELETEResponseBuilder() {
    if(FULL_LOGGING_ENABLED)
        Log::v("DELETE method is requested, we're trying to delete: " + this->path + "...");
    try {
        File::deleteLocation(this->path);
    	this->setStatusCode(204);
        if(FULL_LOGGING_ENABLED) Log::v("Requested file has been deleted successfully!");
    } catch (Utils::WebservException &ex) {
    	this->setStatusCode(500);
        Log::w("DELETE failed due to: " + std::string(ex.what()));
    }
    this->setResponseBody();
    this->setHeaders();
    this->setResponseLine();
    this->fileOffset = -2;
    addDataToResponse(this->responseLine);
    addDataToResponse(this->responseHeaders);
    addDataToResponse(this->responseBody);
    setServingState(true);
}