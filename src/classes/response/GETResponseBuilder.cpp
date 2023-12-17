#include "../../../webserv.hpp"

void Response::GETResponseBuilder() {

    // 	if (this->currentRoute != NULL) {
	// 	// allowed_methods = this->currentRoute->allowed_methods;
	// 	uploadSupport = true;
	// }
    // if (this->request->getRequestLine()["method"] == "POST" && uploadSupport) {
		
	// }
    
    this->setResponseBody();
    this->setHeaders();
    this->setResponseLine();
    addDataToResponse((*this).responseLine);
    addDataToResponse((*this).responseHeaders);
    addDataToResponse((*this).responseBody);
    setServingState(true);
}