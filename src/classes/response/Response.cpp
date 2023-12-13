#include "../../../webserv.hpp"

Response::Response() {
    (*this).path = "";
    (*this).server = NULL;
    (*this).request = NULL;
    (*this).response = "";
    (*this).responseLine = "";
    (*this).responseHeaders = "";
    (*this).responseBody = "";
    (*this).statusCode = 0;
}

Response::~Response() {

}

void Response::setResponseBody() {



}




void Response::GETResponseBuilder() {

    this->setResponseBody();
    this->setHeaders();
    this->setResponseLine();

    this->response = this->responseLine + this->responseHeaders + this->responseBody;
}

void Response::clearResponse() {
    (*this).path = "";
    (*this).response = "";
    (*this).responseLine = "";
    (*this).responseHeaders = "";
    (*this).responseBody = "";
    (*this).statusCode = 0;

}

void Response::setRequest(RequestParser &request) {
    (*this).request = &request;
}

void Response::setServer(t_server &server) {
    (*this).server = &server;
}

std::string Response::getStringStatus(){
    switch(this->statusCode){
        case 200:
            return "200 OK";
        case 404:
            return "404 Not Found";
        case 500:
            return "500 Internal Server Error";
        default:
            return "501 Unknown";
    }
}

void Response::responseBuilder() {

    if (this->request->getRequestLine()["method"] == "GET") {

        this->setPath(request->getRequestLine()["path"]);

        this->GETResponseBuilder();
        
    } else if (this->request->getRequestLine()["method"] == "POST") {

    } else if (this->request->getRequestLine()["method"] == "DELETE") {

    }

}



// Response::Response(int clientFd, const RequestParser &parser){
//     this->clientFD = clientFd;
//     this->requestParser = parser;
//     std::string path = requestParser.getRequestLine()["path"];
//     std::string finalPath = "src/client-side";

// 	std::cout << path << std::endl;

// 	if (File::isDirectory(finalPath + path)) {

// 		std::string indexHTML = path.find_last_of("/") != std::string::npos && path.find_last_of("/") == path.size() -1 ? "index.html" : "/index.html";

// 		if (File::isFile(finalPath + path + indexHTML)) {
// 			finalPath += path + indexHTML;
// 		} else {
// 			bool directory_listing = true; // This is a temporary variable, we should use value form config file
// 			if (directory_listing) {
// 				Log::i("heyyyyyyy");
// 				// autoindex here
// 				finalPath += "/auto_index_template.html";

// 				// handleAutoIndex(finalPath + path);
// 			} else {
// 				this->status = 403;
// 				Log::e("Response: 403 Forbidden");
// 				return;
// 			}
// 		}

// 	} else if (File::isFile(finalPath + path)) {
// 		finalPath += path;
// 	} else {
// 		this->status = 403;
// 		Log::e("Response: 403 Forbidden");
// 		Log::e("path: " + finalPath + path);
// 		return;
// 	}

//     Log::i("Fetching response from \"" + finalPath + "\"...");
//     try {
//         std::string content = File::getFileContent(finalPath);
//         this->response = content;
//         this->status = 200;
//     } catch (std::exception &e) {
//         this->status = 404;
//         Log::e("Response: 404 Not Found");
//     }
//     this->path = finalPath;
// }

// std::string Response::sendResponse() {
//     if(this->status != 200){ // TOFIX: THIS WILL MAKE OTHER FILES SUCH AS CSS AND JS TO BE SENT AS HTML. FIX THIS!
//         this->getErrorPageHTML();
//     }
//     this->contentType = File::getContentType(path);
//     this->contentSize = std::to_string(this->response.length());
//     std::string status = getStringStatus();

//     response = "HTTP/1.1 " + status + " \nContent-Type: " + contentType + " \nContent-Length: " + contentSize + "\n\n" + response;
//     if(send(clientFD, response.c_str(), response.length(), 0) == -1)
//         std::cout << "awiliiii" << std::endl;
//     // this->contentType.clear();
//     // this->contentSize.clear();
//     // response.clear();
//     // path.clear();
//     Log::i("Response sent");
//     Log::d("Response information:\n- Content-Type: " + contentType + "\n- Content-Length: " + contentSize + "\n- Status: " + status + "\n- Path: " + path);
//     return(response);
// }

// std::string Response::getErrorPageHTML(){
//     switch(status){
//         case 404:
//             this->path = "src/client-side/error_pages/404.html";
//             this->response = File::getFileContent(this->path);
//             break;
//         case 500:
//             this->path = "src/client-side/error_pages/500.html";
//             this->response = File::getFileContent(this->path);
//             break;
//         default:
//             this->path = "src/client-side/error_pages/501.html";
//             this->response = File::getFileContent(this->path);
//             break;
//     }
//     return this->response;
// }

// void Response::setPath(const std::string &path){
//     this->path = path;
// }

// const std::string &Response::getResponse(){
//     return this->response;
// }

// const int &Response::getStatus(){
//     return this->status;
// }

// std::string Response::getStringStatus(){
//     switch(this->status){
//         case 200:
//             return "200 OK";
//         case 404:
//             return "404 Not Found";
//         case 500:
//             return "500 Internal Server Error";
//         default:
//             return "501 Unknown";
//     }
// }