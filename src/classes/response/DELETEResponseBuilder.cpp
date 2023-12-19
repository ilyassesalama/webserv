#include "../../../webserv.hpp"

void Response::DELETEResponseBuilder() {
    if(FULL_LOGGING_ENABLED)
        Log::v("DELETE method is request, we're trying to delete: " + this->path + "...");
    try {
        File::deleteFile(this->path);
        if(FULL_LOGGING_ENABLED) Log::v("Requested file has been deleted successfully!");
    } catch (Utils::WebservException &ex) {
        Log::w("DELETE failed due to: " + std::string(ex.what()));
    }
    this->setStatusCode(204);
    this->setResponseBody();
    this->setHeaders();
    this->setResponseLine();
    (*this).fileOffset = -2;
    addDataToResponse(this->responseLine);
    addDataToResponse(this->responseHeaders);
    addDataToResponse(this->responseBody);
    setServingState(true);
}