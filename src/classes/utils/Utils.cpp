#include "../../../webserv.hpp"


Utils::WebservException::WebservException(const std::string& message) throw() : message(message) {}

Utils::WebservException::~WebservException() throw() {}

const char* Utils::WebservException::what() const throw() {
    return message.c_str();
}






std::string readHtmlFile(const std::string& filePath) {
    std::ifstream file(filePath.c_str());

    if (!file.is_open()) {
        // Handle the case where the file cannot be opened
        return "";
    }

    std::ostringstream content;
    std::string line;

    while (std::getline(file, line)) {
        content << line << '\n';
    }

    file.close();

    return content.str();
}

std::string generateHtmlResponse(const std::string& filePath) {
    std::string htmlContent = readHtmlFile(filePath);

    if (htmlContent.empty()) {
        // Handle the case where the HTML file content is empty or cannot be read
        return "";
    }

    // Construct the HTTP response
    std::ostringstream response;
    response << "HTTP/1.1 200 OK\r\n";
    response << "Content-Type: text/html\r\n";
    response << "Content-Length: " << htmlContent.length() << "\r\n";
    response << "\r\n";  // Empty line separates headers and body
    response << htmlContent;
    return response.str();
}


