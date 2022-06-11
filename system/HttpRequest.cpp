/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cshells <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/31 16:06:18 by cshells           #+#    #+#             */
/*   Updated: 2022/01/31 16:06:19 by cshells          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"

int HttpRequest::id_generator = 0;

HttpRequest::HttpRequest(const string &type, const string &uri, const string &protocol)
    : HttpMessage(protocol), _id(id_generator++), _type(type), _uri(uri), _size(0), _headerSize(0),
    _contentLength(0), _bufDataBegin(nullptr), _bufDataEnd(nullptr), _bufDataSize(0), _handledDataSize(0),
    _chunked(false), _fullReceived(false), _fullProcessed(false), _keepAlive(false) {

    if (type != "POST")
        _fullReceived = true;

    addHeaderEntry("Request-Type", type);
    addHeaderEntry("Request-Uri", uri);
}

HttpRequest::~HttpRequest() {}

int HttpRequest::countCRLFSequenceLenAtStart(const char *begin, const char *end) {

    int   i = 0;
    int   range = end - begin;

    if (*begin == '\r') {
        for (; i < range && begin[i] == '\r' && begin[i + 1] == '\n'; i += 2) {}
    }

    return i;
}

bool HttpRequest::isRequestLineValid(const char *request_begin) {

    const char   *end = strchr(request_begin, '\r');
    if (!end)
        return false;

    const char   *first_space = strchr(request_begin, ' ');
    if (!first_space || !(first_space + 1))
        return false;

    const char   *second_space = strchr(first_space + 1, ' ');
    if (!second_space || !(second_space + 1))
        return false;

    for (; *request_begin != ' '; ++request_begin) {
        if (!isupper(*request_begin))
            return false;
    }
    ++request_begin;
    if (*request_begin != '/')
        return false;

    size_t   len = end - second_space - 1;

    if (strncmp(second_space + 1, "HTTP/1.0", len) && strncmp(second_space + 1, "HTTP/1.1", len))
        return false;

    return true;
}

string HttpRequest::defineRequestType(const char *begin) {
    if (!strncmp(begin, "GET", strlen("GET")))
        return "GET";
    else if (!strncmp(begin, "POST", strlen("POST")))
        return "POST";
    else if (!strncmp(begin, "DELETE", strlen("DELETE")))
        return "DELETE";

    return "";
}



bool HttpRequest::initHeader(const char *headerBegin, const char *headerEnd) {

    string           header = string(headerBegin, headerEnd);
    vector<string>   headerEntries = Utils::split(header, "\r\n");

    headerEntries.erase(headerEntries.cend() - 1); // TODO: Optimization
    for (vector<string>::iterator entryIter = headerEntries.begin(); entryIter != headerEntries.end(); ++entryIter) {
        pair<string, string> headerEntry = splitHeaderOptionToKeyValue((*entryIter));
        if (headerEntry.first.empty()) {
            std::cout << "HttpRequest: Fail: Invalid header entry format" << std::endl;
            return false;
        }
        addHeaderEntry(headerEntry);
    }

    return true;
}

void HttpRequest::initData(const char *requestBegin, const char *headerEnd, const char *bufferEnd) {

    _bufDataBegin = headerEnd;
    _bufDataEnd = HttpMessage::findMessageEnd(_bufDataBegin);
    _bufDataSize = _bufDataEnd - _bufDataBegin;
    if (_bufDataEnd)
        _fullReceived = true;
    else
        _fullReceived = false;
}

void HttpRequest::initParams(const char *requestBegin, const char *headerEnd) {

    string   contentLengthString = getHeaderEntryValue("Content-Length");

    _contentLength = std::strtol(contentLengthString.c_str(), 0, 10);
    _keepAlive = (getHeaderEntryValue("Connection") == "keep-alive");
    _chunked = (getHeaderEntryValue("Transfer-Encoding") == "chunked");

    size_t headerSize = headerEnd - requestBegin;
    _size = headerSize + _bufDataSize;

    std::cout << "HttpRequest: Request size: " << _size << std::endl;
}

HttpRequest *HttpRequest::createRequest(const char* &bufferBegin, const char* &bufferEnd, const WebServer* server) {

    std::cout << "HttpRequest: Creating request" << std::endl;

    int          numberCharsIgnore;
    const char   *requestBegin;
    string       type;
    const char   *headerEnd;

    numberCharsIgnore = countCRLFSequenceLenAtStart(bufferBegin, bufferEnd);
    requestBegin = bufferBegin + numberCharsIgnore;
    std::cout << "HttpRequest: Checking request line" << std::endl;
    if (!isRequestLineValid(requestBegin)) {
        std::cout << "HttpRequest: Error: Request line is invalid" << std::endl;
        return nullptr;
        // 400 BAD REQUEST
    }
    type = defineRequestType(requestBegin);
    if (type.empty()) {
        std::cout << "HttpRequest: Error: Request type is invalid" << std::endl;
        return nullptr;
        // 405 METHOD NOT ALLOWED
    }
    headerEnd = findMessageEnd(requestBegin);
    if (headerEnd - requestBegin > MAX_ENTITY_SIZE) {
        std::cout << "HttpRequest: Error: Entity too large" << std::endl;
        return nullptr;
        // 413 ENTITY TO LARGE
    }

    const string   uri = defineRequestUri(requestBegin);
    const char     *headerBegin = strchr(requestBegin, '\n') + 1;
    const string   hostname = getOptionValueFromBuffer(headerBegin, "Host");

    if (hostname.empty()) {
        std::cout << "HttpRequest: Error: Hostname is invalid" << std::endl;
        return nullptr;
        // 400 BAD REQUEST
    }

    const ServerOptions   *urlOptions = server->getLocationOptions(hostname, uri);

    if (!urlOptions->isMethodAllowed(type)) {
        std::cout << "HttpRequest: Error: Method not allowed" << std::endl;
        return nullptr;
        // 405 METHOD NOT ALLOWED
    }

    string                   protocol = defineRequestProtocol(requestBegin);
    HttpRequest              *request = new HttpRequest(type, uri, protocol);
    const LocationOptions*   location = dynamic_cast<const LocationOptions*>(urlOptions);

    if (location) { // todo надо попроще
        const string   &redirect = location->getReturnValue();
        request->setRedirect(redirect);
        std::cout << "HttpRequest: Redirect value: " << request->getRedirect() << std::endl;
    }

    bool   headerInitSuccess = request->initHeader(headerBegin, headerEnd);

    if (!headerInitSuccess) {
        std::cout << "HttpRequest: Error: Header initializing" << std::endl;
        delete request;
        return nullptr;
    }
    if (type == "POST")
        request->initData(requestBegin, headerEnd, bufferEnd);
    request->initParams(requestBegin, headerEnd);

    return request;
}

pair<string, string> HttpRequest::splitHeaderOptionToKeyValue(const string &str) {

    size_t   separator_pos = str.find(": ", 0);

    if (separator_pos == string::npos)
        return std::make_pair("", "");

    string   key = str.substr(0, separator_pos);
    string   value = str.substr(separator_pos + strlen(": "));

    return make_pair(key, value);
}

string HttpRequest::defineRequestUri(const char *begin) {

    string       result_path;
    const char   *first_space_pos;
    const char   *second_space_pos;

    std::cout << "HttpRequest: Defining request path..." << std::endl;
    if (begin) {
        first_space_pos = strchr(begin, ' ');
        if (!first_space_pos || !(first_space_pos + 1))
            return "";
        second_space_pos = strchr(first_space_pos + 1, ' ');
        if (!second_space_pos)
            return "";
        result_path.assign(first_space_pos + 1, second_space_pos);
    }
    return result_path;
}

string HttpRequest::defineRequestProtocol(const char *request_begin) {

    const char *begin = strstr(request_begin, "HTTP/");

    return string(begin, strlen("HTTP/1.*"));
}

std::string HttpRequest::getOptionValueFromBuffer(const char *buffer, const char *optionName) {

    const char   *optionBegin;
    const char   *separatorBegin;
    const char   *optionValueEnd;
    size_t       optionLen = strlen(optionName);

    if (!buffer || !optionName)
        return "";

    optionBegin = strstr(buffer, optionName);
    if (!optionBegin || !(optionBegin + optionLen))
        return "";
    separatorBegin = strstr(optionBegin, ": ");
    if (!separatorBegin)
        return "";
    optionValueEnd = strstr(optionBegin + optionLen, "\r\n");
    if (!optionValueEnd)
        return "";

    return std::string(separatorBegin + strlen(": "), optionValueEnd);
}

void HttpRequest::setDataBegin(const char *data_begin) { _bufDataBegin = data_begin; }

void HttpRequest::setDataEnd(const char *data_end) { _bufDataEnd = data_end; }

void HttpRequest::setBuffedDataSize(size_t size) { _bufDataSize = size; }

void HttpRequest::setHandledDataSize(size_t size) { _handledDataSize = size; }

void HttpRequest::setDataPath(const string &path) { _dataPath = path; }

void HttpRequest::setContentType(const std::string &contentType) { _contentType = contentType; }

void HttpRequest::setContentLength(const long long &contentLength) { _contentLength = contentLength; }

void HttpRequest::setUri(const string &uri) { _uri = uri; }

void HttpRequest::setRedirect(const string &redirect) { _redirect = redirect; }

void HttpRequest::setSize(size_t size) { _size = size; }

void HttpRequest::setChunked(bool value) { _chunked = value; }

void HttpRequest::setFullReceived(bool value) { _fullReceived = value; }

void HttpRequest::setKeepAlive(bool value) { _keepAlive = value; }

void HttpRequest::setFullProcessed(bool value) { _fullProcessed = value; }


const std::string &HttpRequest::getType() const { return _type; }

const char *HttpRequest::getDataBegin() const { return _bufDataBegin; }

const char *HttpRequest::getDataEnd() const { return _bufDataEnd; }

size_t HttpRequest::getBuffedDataSize() const { return _bufDataSize; }

size_t HttpRequest::getHandledDataSize() const { return _handledDataSize; }

const std::string &HttpRequest::getDataPath() const { return _dataPath; }

const std::string &HttpRequest::getContentType() const { return _contentType; }

const long long  HttpRequest::getContentLength() const {
    const string&   value = getHeaderEntryValue("Content-Length");

    if (value.empty())
        return -1;

    long long   content_length = std::atol(value.c_str()); // TODO: try carch
    //        content_length = strtoll(header_begin, )
    //        content_length = std::wcstol(extractParam(header_begin, "Content-Length: "), 0, 10); // try catch nedded

    return content_length;
}

const size_t HttpRequest::getHeaderSize() const { return _headerSize; }

const size_t HttpRequest::getSize() const { return _size; }

bool HttpRequest::isFullReceived() const { return _fullReceived; }

const size_t HttpRequest::getRemainingDataSize() const { return _contentLength - _handledDataSize;  }

const string &HttpRequest::getUri() const { return _uri; }

const string &HttpRequest::getRedirect() const { return _redirect; }

bool HttpRequest::isRedirect() const { return !_redirect.empty(); }

bool HttpRequest::isKeepAlive() const { return _keepAlive; }

bool HttpRequest::isChunked() const { return _chunked; }

int HttpRequest::getId() const { return _id; }

void HttpRequest::increaseHandledDataSize(size_t value) { _handledDataSize += value; }

bool HttpRequest::isFullProcessed() const { return _fullProcessed; }