#include "../../../webserv.hpp"

void uploadRequest() {

}

bool isTransferEncoding(std::map<std::string, std::string>&headers) {
	if (headers.find("Transfer-Encoding") != headers.end()) {
		return headers["Transfer-Encoding"] == "chunked" ? true : false;
	}
	return false;
}

void Response::POSTResponseBuilder() {
	// std::string contentType = getContentType(this->request->getHeaders(), this->path);
	// bool transfer_encoding = isTransferEncoding(this->request->getHeaders());
	// if (contentType == "multipart/form-data") {
	// 	// parse form-data
	// 	std::string boundary = getBoundary();
	// } else {



	// 	if (transfer_encoding) {

	// 	}

	// }

	this->setResponseBody();
    this->setHeaders();
    this->setResponseLine();

    this->response = this->responseLine + this->responseHeaders + this->responseBody;
}