#pragma once

#include "../../webserv.hpp"

enum PrasingState {
    REQ_PARSER_STARTED,
    REQ_PARSER_HEAD_LINE_PENDING,
    REQ_PARSER_HEAD_LINE_OK,
    REQ_PARSER_HEADS_PENDING,
    REQ_PARSER_HEADS_OK,
    REQ_PARSER_BODY_PENDING,
    REQ_PARSER_BODY_OK,
    REQ_PARSER_OK,
    REQ_PARSER_FAILED
};

class RequestParser {
    private:
        PrasingState parsingState;
        std::map<std::string, std::string> requestLine;
        std::map<std::string, std::string> headers;
        std::map<std::string, std::string> params;
        std::string body;
    public:
        RequestParser();
        RequestParser(const std::string &requestData);

        void nullOutVars();
        void logParsedRequest();

        void initRequestParser(std::string &requestData);
        void setParsingState(PrasingState state);
        void parseRequestLine(std::string &requestData);
        void parseRequestHeaders(std::string &requestData);
        void parseRequestParams(std::string &requestData);
        void parseRequestBody(std::stringstream &httpStream);

        std::map<std::string, std::string> &getRequestLine(); // this isn't const because i want to call it a bit diffrently
        const std::map<std::string, std::string> &getHeaders();
        const std::map<std::string, std::string> &getParams();
        const std::string &getBody();
        const PrasingState &getParsingState();
};