#include "../../../webserv.hpp"

void uploadRequest() {

}

bool isTransferEncoding(std::map<std::string, std::string>&headers) {
	if (headers.find("Transfer-Encoding") != headers.end()) {
		return headers["Transfer-Encoding"] == "chunked" ? true : false;
	}
	return false;
}

std::string getBody(const std::string &body) {
	size_t sizePos = body.find("0123456789");
	if (sizePos == std::string::npos)
	 return "";

	size_t sizeEndPos = sizePos;
	for (sizeEndPos = sizePos; sizeEndPos < body.size(); sizeEndPos++) {
		if (!isdigit(body[sizeEndPos]))
			break ;
	}

	std::string bodySize = body.substr(sizePos, sizeEndPos);

	return body.substr(sizeEndPos);
}

void Response::POSTResponseBuilder() {

	// bool isRequestFinished = false; // should be in class with false as default
	// bool isTransferEncoding = this->request->getHeaders().find("Transfer-Encoding") != this->request->getHeaders().end() ? true : false;

	// std::string body; // this should be in class

	// if (!isRequestFinished && isTransferEncoding) {
	// 	std::cout << "HEEY" << std::endl;
	// 	this->setResponseBody();
	// 	// collect in a buffer
	// 	body += getBody(this->request->getBody());
	// }
	// if (isRequestFinished) {
	// 	// save file
	// }

	// if (!isTransferEncoding)
		// this->setResponseBody();
	this->responseBody = "";
    this->setHeaders();
    this->setResponseLine();
	addDataToResponse(this->responseLine);
    addDataToResponse(this->responseHeaders);
    addDataToResponse(this->responseBody);
    setServingState(true);
}