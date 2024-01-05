#include "../../../webserv.hpp"

void Response::DELETEResponseBuilder() {
    if(FULL_LOGGING_ENABLED)
        Log::v("DELETE method is requested, we're trying to delete: " + this->path + "...");
    if(File::deleteLocation(this->path)) {
    	this->setStatusCode(204);
        if(FULL_LOGGING_ENABLED) Log::v("Requested file has been deleted successfully!");
    } else {
    	this->setStatusCode(500);
    }
    this->setHeaders();
    this->setResponseLine();
    this->responseBody = "";
    this->fileOffset = -2;
    addDataToResponse(this->responseLine);
    addDataToResponse(this->responseHeaders);
    addDataToResponse(this->responseBody);
    setServingState(true);
}