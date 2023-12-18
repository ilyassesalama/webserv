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
	// std::string contentType = getContentType(this->request->getHeaders(), this->path);
	// bool transfer_encoding = isTransferEncoding(this->request->getHeaders());
	// if (contentType == "multipart/form-data") {
	// 	// parse form-data
	// 	std::string boundary = getBoundary();
	// } else {



	// 	if (transfer_encoding) {

	// 	}

	// }

	// bool isRequestFinished; // should be in class with false as default
	// bool isTransferEncoding = std::find(this->request->getHeaders().begin(), this->request->getHeaders().end(), this->request->getHeaders()["Transfer-Encoding"]) != this->request->getHeaders().end() ? true : false;

	// std::string body; // this should be in class

	// if (!isRequestFinished && isTransferEncoding) {
	// 	// collect in a buffer
	// 	body += getBody(this->request->getBody());
	// }
	// if (isRequestFinished) {
	// 	// save file
	// }

	this->setResponseBody();
    this->setHeaders();
    this->setResponseLine();

    this->response = this->responseLine + this->responseHeaders + this->responseBody;
}