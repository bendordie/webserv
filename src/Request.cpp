/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cshells <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/31 16:06:18 by cshells           #+#    #+#             */
/*   Updated: 2022/01/31 16:06:19 by cshells          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

int Request::idGenerator = 0;

Request::Request(int responseCode, const string& protocol)
    : HttpMessage(protocol) {

    _id = idGenerator++;
    _responseCode = responseCode;

    _redirectCode = 0;
    _size = 0;
    _headerSize = 0;
    _contentLength = 0;

    _bufDataBegin = nullptr;
    _bufDataEnd = nullptr;
    _bufDataSize = 0;
    _handledDataSize = 0;

    _chunked = false;
    _fullReceived = true;
    _fullProcessed = true;
    _keepAlive = false;
    _cgiHandled = false;
    _acceptEncoding = false;
}

Request::Request(const string& method, const string& url, const string& protocol)
    : HttpMessage(protocol) {

    _id = idGenerator++;
    _responseCode = 0;
    _method = method;
    _url = url;

    _redirectCode = 0;
    _size = 0;
    _headerSize = 0;
    _contentLength = 0;

    _bufDataBegin = nullptr;
    _bufDataEnd = nullptr;
    _bufDataSize = 0;
    _handledDataSize = 0;

    _chunked = false;
    _fullReceived = false;
    _fullProcessed = false;
    _keepAlive = false;
    _cgiHandled = false;
    _acceptEncoding = false;

    if (method != "POST")
        _fullReceived = true;

    addHeaderEntry("Request-Method", method);
    addHeaderEntry("Request-URL", url);
}

Request::~Request() {}

Request *Request::createRequest(const char* & bufferBegin, const char* & bufferEnd, int responseCode) {

    (void)bufferEnd;

    string protocol = findHttpProtocolFromBuffer(bufferBegin);

    if (protocol.empty())
        protocol = "HTTP/1.0";

    return new Request(responseCode, protocol);
}

// Request URL:    /directory/resources
// Root:           /cshells/Desktop/git_webserv/www/default_site
// Result path:    /cshells/Desktop/git_webserv/www/default_site/resources

// Request URL:    /directory/resources/img/image.png
// Root:           /cshells/Desktop/git_webserv/www/default_site
// Result path:    /cshells/Desktop/git_webserv/www/default_site/resources/img/image.png

// Request URL:    /
// Root:           /cshells/Desktop/git_webserv/www/default_site
// Result path:    /cshells/Desktop/git_webserv/www/default_site/

// Request URL:    /directory
// Root:           /cshells/Desktop/git_webserv/www/default_site
// Result path:    /cshells/Desktop/git_webserv/www/default_site/

Request *Request::createRequest(const char* &bufferBegin, const char* &bufferEnd, const WebServer* server) {

    showDebugMessage("Request: Creating request...");

    int           numberCharsIgnore;
    const char*   requestBegin;

    numberCharsIgnore = countCRLFSequenceLenAtStart(bufferBegin, bufferEnd);
    requestBegin = bufferBegin + numberCharsIgnore;
    if (!isRequestLineValid(requestBegin)) {
        showDebugMessage("Request: Error: Request line is invalid");
        return new Request(global::response_status::BAD_REQUEST, "HTTP/1.0");
        // 400 BAD REQUEST
    }
    showDebugMessage("Request: Request line is valid");

    string   method = defineRequestMethod(requestBegin);
    if (method.empty()) {
        showDebugMessage("Request: Error: Request method is invalid");
        return new Request(global::response_status::NOT_ALLOWED, "HTTP/1.0");
        // 405 METHOD NOT ALLOWED
    }
    showDebugMessage("Request: Method: " + method);

    string         protocol = findHttpProtocolFromBuffer(requestBegin);
    const string   url = defineRequestURL(requestBegin);
    const char*    headerBegin = strchr(requestBegin, '\n') + 1;
    const string   hostname = getOptionValueFromBuffer(headerBegin, "Host");

    if (hostname.empty()) {
        showDebugMessage("Request: Error: Hostname is invalid");
        return new Request(global::response_status::BAD_REQUEST, protocol);;
        // 400 BAD REQUEST
    }

    showDebugMessage("Request: URL: " + url);
    showDebugMessage("Request: Hostname: " + hostname);
    showDebugMessage("Request: Protocol: " + protocol);

    const Location*   location = server->getLocation(hostname, url);

    if (!location->isMethodAllowed(method)) {
        showDebugMessage("Request: Error: Method not allowed");
        return new Request(global::response_status::NOT_ALLOWED, protocol);
        // 405 METHOD NOT ALLOWED
    }

    long          contentLength = atol(getOptionValueFromBuffer(bufferBegin, "Content-Length").c_str());
    const char*   headerEnd = findMessageEnd(requestBegin);
    long          clientMaxBodySize = location->getClientBodySize();

    showDebugMessage("Request: Client max body size: " + to_string(clientMaxBodySize));
    if (contentLength > clientMaxBodySize || (bufferEnd - headerEnd) > clientMaxBodySize) {
        showDebugMessage("Request: Error: Entity too large");
        return new Request(global::response_status::ENTITY_TOO_LARGE, protocol);
        // 413 ENTITY TO LARGE
    }

    auto   request = new Request(method, url, protocol);
    request->setLocation(location);

    const string&   redirectString = location->getReturnValue();
    if (!redirectString.empty()) {
        int             redirectCode = stoi(redirectString.substr(0, 3));  // todo redirect parsing before starting server
        const string&   redirectURL = redirectString.substr(4);

        request->setRedirectCode(redirectCode);
        request->setRedirectURL(redirectURL);

        showDebugMessage("Request: Redirect code: " + to_string(request->getRedirectCode()));
        showDebugMessage("Request: Redirect URL: " + request->getRedirectURL());
    }

    bool   acceptEncoding = !(getOptionValueFromBuffer(headerBegin, "Accept-Encoding").empty());

    request->setAcceptEncodingFlag(acceptEncoding);

    showDebugMessage("Request: Location URL: " + location->getURL());
    if (location->isCgiSupport()) {
        showDebugMessage("Request: Location supports CGI");
        const string&   cgiName = location->getCgiName();
        const char*     tailURL = Utils::reverse_strstr(url.c_str(), cgiName.c_str());

        if (tailURL)
            request->setHandledByCgiFlag(true);
    }
    showDebugMessage("Request: CGI flag set to " + to_string(request->isHandledByCgi()));

    showDebugMessage("Request: Initializing header...");
    bool   headerInitSuccess = request->initHeader(headerBegin, headerEnd);
    if (!headerInitSuccess) {
        showDebugMessage("Request: Error: Header initializing");
        return new Request(global::response_status::BAD_REQUEST, protocol);
    }

    const string   contentType = getOptionValueFromBuffer(bufferBegin, "Content-Type");

    if (contentLength > 0 && !contentType.empty()) {
        showDebugMessage("Request: Initializing data...");

        request->setContentType(contentType);
        request->setContentLength(contentLength);

        showDebugMessage("Request: Content-Length: " + to_string(request->getContentLength()));
        showDebugMessage("Request: Content-Type: " + request->getContentType());

        request->initData(headerEnd, bufferEnd);
    }
    request->initParams(requestBegin, headerEnd);

    return request;
}

bool Request::initHeader(const char *headerBegin, const char *headerEnd) {

    string           header = string(headerBegin, headerEnd);
    vector<string>   headerEntries = Utils::split(header, "\r\n");

    for (vector<string>::iterator entryIter = headerEntries.begin(); entryIter != headerEntries.end(); ++entryIter) {
        pair<string, string> headerEntry = splitHeaderOptionToKeyValue((*entryIter));
//        showDebugMessage("key: " + headerEntry.first + " & value: " + headerEntry.second);
        if (headerEntry.first.empty()) {
            std::cerr << "Request: Fail: Invalid header entry format" << std::endl;
            return false;
        }
        addHeaderEntry(headerEntry);
    }

    return true;
}

void Request::initData(const char *headerEnd, const char *bufferEnd) {

    size_t   remainBufferSize = bufferEnd - headerEnd;

    _bufDataBegin = headerEnd; // todo: full received переделать (что если отправляется данных на CGI больше, чем вмещает основной буфер)

    if (static_cast<long long>(remainBufferSize) <= _contentLength) {
        _fullReceived = true;
        _bufDataEnd = _bufDataBegin + _contentLength;
        _bufDataSize = _contentLength;
    } else {
        _fullReceived = false;
        _bufDataEnd = bufferEnd;
        _bufDataSize = remainBufferSize;
        _handledDataSize = remainBufferSize;
    }
    showDebugMessage("Request: Init data: _bufDataSize: " + to_string(_bufDataSize));
}

void Request::initParams(const char *requestBegin, const char *headerEnd) {

    string   contentLengthString = getHeaderEntryValue("Content-Length");

    showDebugMessage("Request: Content length in string: " + contentLengthString);

    _contentLength = std::strtol(contentLengthString.c_str(), 0, 10);

    showDebugMessage("Request: Content length in integer: " + to_string(_contentLength));
    _keepAlive = (getHeaderEntryValue("Connection") == "keep-alive");
    _chunked = (getHeaderEntryValue("Transfer-Encoding") == "chunked");

    size_t   headerSize = headerEnd - requestBegin;
    _size = headerSize + _bufDataSize;

    showDebugMessage("Request: Request size: " + to_string(_size));
}

int Request::countCRLFSequenceLenAtStart(const char *begin, const char *end) {

    int   i = 0;
    int   range = end - begin;

    if (*begin == '\r') {
        for (; i < range && begin[i] == '\r' && begin[i + 1] == '\n'; i += 2) {}
    }

    return i;
}

bool Request::isRequestLineValid(const char *requestBegin) {

    const char*   end = strchr(requestBegin, '\r');
    if (!end)
        return false;

    const char*   firstSpace = strchr(requestBegin, ' ');
    if (!firstSpace || !(firstSpace + 1))
        return false;

    const char*   secondSpace = strchr(firstSpace + 1, ' ');
    if (!secondSpace || !(secondSpace + 1))
        return false;

    for (; *requestBegin != ' '; ++requestBegin) {
        if (!isupper(*requestBegin))
            return false;
    }
    ++requestBegin;
    if (*requestBegin != '/')
        return false;

    size_t   len = end - secondSpace - 1;

    if (strncmp(secondSpace + 1, "HTTP/1.0", len) && strncmp(secondSpace + 1, "HTTP/1.1", len))
        return false;

    return true;
}

string Request::defineRequestMethod(const char *begin) {
    if (!strncmp(begin, "GET", strlen("GET")))
        return "GET";
    else if (!strncmp(begin, "POST", strlen("POST")))
        return "POST";
    else if (!strncmp(begin, "DELETE", strlen("DELETE")))
        return "DELETE";

    return "";
}

pair<string, string> Request::splitHeaderOptionToKeyValue(const string &str) {

    size_t   separator_pos = str.find(": ", 0);

    if (separator_pos == string::npos)
        return std::make_pair("", "");

    string   key = str.substr(0, separator_pos);
    string   value = str.substr(separator_pos + strlen(": "));

    return make_pair(key, value);
}

string Request::defineRequestURL(const char *begin) {

    string        resultPath;
    const char*   firstSpacePos;
    const char*   secondSpacePos;

    showDebugMessage("Request: Defining request path...");

    if (begin) {
        firstSpacePos = strchr(begin, ' ');
        if (!firstSpacePos || !(firstSpacePos + 1))
            return "";
        secondSpacePos = strchr(firstSpacePos + 1, ' ');
        if (!secondSpacePos)
            return "";
        resultPath.assign(firstSpacePos + 1, secondSpacePos);
    }
    return resultPath;
}

int Request::getResponseCode() const { return _responseCode; }

const std::string &Request::getMethod() const { return _method; }

const Location *Request::getLocation() const { return _location; }

const char *Request::getBuffedDataBegin() const { return _bufDataBegin; }

const char *Request::getBuffedDataEnd() const { return _bufDataEnd; }

size_t Request::getBuffedDataSize() const { return _bufDataSize; }

size_t Request::getHandledDataSize() const { return _handledDataSize; }

const std::string &Request::getDataPath() const { return _dataPath; }

const std::string &Request::getContentType() const { return _contentType; }

long long  Request::getContentLength() const {
    const string&   value = getHeaderEntryValue("Content-Length");

    if (value.empty())
        return -1;

    long long   contentLength = std::atol(value.c_str()); // TODO: try catch?

    return contentLength;
}

size_t Request::getHeaderSize() const { return _headerSize; }

size_t Request::getSize() const { return _size; }

bool Request::isFullReceived() const { return _fullReceived; }

size_t Request::getRemainingDataSize() const { return _contentLength - _handledDataSize;  }

const string &Request::getURL() const { return _url; }

int Request::getRedirectCode() const { return _redirectCode; }

const string &Request::getRedirectURL() const { return _redirectURL; }

bool Request::isRedirect() const { return _redirectCode != 0; }

bool Request::isKeepAlive() const { return _keepAlive; }

bool Request::isChunked() const { return _chunked; }

int Request::getId() const { return _id; }

bool Request::isFullProcessed() const { return _fullProcessed; }

bool Request::isHandledByCgi() const { return _cgiHandled; }

bool Request::isAcceptEncoding() const { return _acceptEncoding; }


void Request::setResponseCode(int responseCode) { _responseCode = responseCode; }

void Request::setLocation(const Location *location) { _location = location; }

void Request::increaseHandledDataSize(size_t value) { _handledDataSize += value; }

void Request::setDataBegin(const char *data_begin) { _bufDataBegin = data_begin; }

void Request::setDataEnd(const char *data_end) { _bufDataEnd = data_end; }

void Request::setBuffedDataSize(size_t size) { _bufDataSize = size; }

void Request::setHandledDataSize(size_t size) { _handledDataSize = size; }

void Request::setDataPath(const string &path) { _dataPath = path; }

void Request::setContentType(const std::string &contentType) { _contentType = contentType; }

void Request::setContentLength(const long long contentLength) { _contentLength = contentLength; }

void Request::setURL(const string &url) { _url = url; }

void Request::setRedirectCode(const int code) { _redirectCode = code; }

void Request::setRedirectURL(const string &url) { _redirectURL = url; }

void Request::setSize(size_t size) { _size = size; }

void Request::setChunked(bool value) { _chunked = value; }

void Request::setFullReceived(bool value) { _fullReceived = value; }

void Request::setKeepAlive(bool value) { _keepAlive = value; }

void Request::setFullProcessed(bool value) { _fullProcessed = value; }

void Request::setHandledByCgiFlag(bool value) { _cgiHandled = value; }

void Request::setAcceptEncodingFlag(bool value) { _acceptEncoding = value; }