#include "../../../webserv.hpp"

Response::Response() {
    (*this).clientSidePath = "/Users/bel-kala/Desktop/webserv/src/client-side";
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

std::string Response::getErrorPageHTML(){

    std::string responseBody;

    switch(this->statusCode){
        case 404:
            this->path = "src/client-side/error_pages/404.html";
            responseBody = File::getFileContent(this->path);
            break;
        case 500:
            this->path = "src/client-side/error_pages/500.html";
            responseBody = File::getFileContent(this->path);
            break;
        default:
            this->path = "src/client-side/error_pages/501.html";
            responseBody = File::getFileContent(this->path);
            break;
    }
    return responseBody;
}

void Response::handleDirectoryRequest() {

    std::string rootPath = "src/client-side"; // temporary

    size_t slashPos = rootPath.find_last_of("/");

    std::string indexHTML = slashPos != rootPath.size() - 1 ? "/index.html" : "index.html";

    if (File::isFile(rootPath + indexHTML)) {
        this->responseBody = File::getFileContent(rootPath + indexHTML);
    } else {
        bool directory_listing = false; // temporary

        if (directory_listing) {
            // response should be list of files in the directory
        } else {
            this->statusCode = 403;
            this->responseBody = this->getErrorPageHTML();
        }
    }
}

void Response::handleFileRequest() {

    std::string rootPath = "src/client-side/index.html"; // temporary

    std::vector<std::string>cgi_methods; // temporary
    cgi_methods.push_back("GET"); // temporary
    std::string cgi_extension = "php"; // temporary

    if (cgi_methods.size() != 0) {
        
        if (!cgi_extension.empty() && std::find(cgi_methods.begin(), cgi_methods.end(), "GET") != cgi_methods.end()) {

        }

    } else {
        this->responseBody = File::getFileContent(rootPath);
    }

}

void Response::setResponseLine() {

    std::string status = getStringStatus();

    this->responseLine = "HTTP/1.1 " + status;
}

void Response::setResponseBody() {

    std::string rootPath = "src/client-side"; // temporary

    if (File::isDirectory(rootPath)) {
        this->handleDirectoryRequest();
    } else if (File::isFile(rootPath)) {
        this->handleFileRequest();
    } else {
        this->statusCode = 301;
        this->responseBody = this->getErrorPageHTML();
    }
}

void Response::setTroute() {
    (*this).routes = server->routes;
}

std::string getPathLocation(std::string path) {
    std::string locationPath;
    locationPath.append("/");
    size_t found = path.substr(1).find_first_of("/");
    if(found != std::string::npos) {
        for(size_t i = 1; i < path.length(); i++) {
            if(path[i] == '/')
                break;
            locationPath += path[i];
        }
    }
    return(locationPath);
}

void Response::setPath(std::string path) {
    std::string locationPath = getPathLocation(path);

    //check if that location ex in the config file
    for(std::vector<t_route>::iterator it = (*this).routes.begin(); it != (*this).routes.end(); ++it) {
        if(locationPath == it->path) {
            //check if the methode is allowed on that location
            if(std::find(it->allowed_methods.begin(),it->allowed_methods.end(),"GET") != it->allowed_methods.end()) {
                std::string requestPath = it->root;
                requestPath.append(path);
                (*this).path =  clientSidePath.append(requestPath);  
                return;
            }
            (*this).statusCode = 405;
        }
    }
    (*this).statusCode = 404;
}


void Response::GETResponseBuilder() {

    // this->setResponseBody();
    // this->setHeaders();
    // this->setResponseLine();

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
    setTroute();
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