/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cshells <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/31 19:40:20 by cshells           #+#    #+#             */
/*   Updated: 2022/01/31 19:40:21 by cshells          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

// SIMPLE (DEFAULT)
Response::Response(const string &protocol, int statusCode, bool keepAliveFlag,
                   bool acceptEncodingFlag, const PairList& cookies, const string &redirectURL)
        : HttpMessage(protocol), _statusCode(statusCode), _contentType("text/html"), _keepAliveFlag(keepAliveFlag) {

    showDebugMessage("Response: SIMPLE constructor");

    auto   result = global::responseStatuses.find(statusCode);
    if (result != global::responseStatuses.end()) {
        _statusMsg = result->second;
    } else {
        _statusCode = 500;
        _statusMsg = "INTERNAL SERVER ERROR";
    }

    showDebugMessage("Response: Code: " + to_string(_statusCode) + " Message: " + _statusMsg);

    const string   &currentTime = Utils::getTimeInString();
    const string   &page = makeStatusPage();
    bool           chunked = (_protocol == "HTTP/1.1") && acceptEncodingFlag;

    _contentLength = page.size();
    makeResponseHeader(chunked, currentTime, cookies, redirectURL);
    setData(_header.begin().base(), _header.end().base());
    if (chunked)
        appendChunkedData(page.begin().base(), page.end().base(), true);
    else
        appendData(page.begin().base(), page.end().base());

    showDebugMessage("Response: Page size: " + to_string(page.size()));
    showDebugMessage("Response: Data size: " + to_string(_dataSize));

    _fullProcessed = true;
}

// CHUNKED
Response::Response(const string &protocol, const string &dataPath, size_t dataBytesProcessed)
    : HttpMessage(protocol), _contentType("text/html"), _dataBytesHandled(dataBytesProcessed) {

    showDebugMessage("Response: CHUNKED constructor");

    if (dataPath.empty()) {
        // ???
    }
    // 65535
    Utils::t_file   file = Utils::readFile(dataPath, global::FILE_READING_BUFFER_SIZE,dataBytesProcessed); // TODO: func for calculating suitable chunk size
    if (!file.data) {
        std::cerr << "Response: Reading file error" << std::endl;
        // ??? throw exception
    }

    setChunkedData(file.data, file.data + file.bytes_read, file.eof);
    _dataBytesHandled = file.bytes_read;
    _fullProcessed = file.eof;
    delete file.data;
}

// FILE
Response::Response(const string &protocol, int statusCode, bool keepAliveFlag, bool acceptEncodingFlag,
                   const string &dataPath, const map<string, string> &contentTypes, const PairList& cookies)
    : HttpMessage(protocol), _statusCode(statusCode), _dataBytesHandled(0),
      _keepAliveFlag(keepAliveFlag), _fullProcessed(false) {

    showDebugMessage("Response: FILE constructor: code = " + to_string(statusCode));

    auto   result = global::responseStatuses.find(statusCode);
    if (result != global::responseStatuses.end()) {
        _statusMsg = result->second;
    } else {
        _statusCode = 500;
        _statusMsg = "INTERNAL SERVER ERROR";
    }

    string          fileTime;
    long            serverMaxBodySize = global::MAX_SERVER_BODY_SIZE; // TODO: hardcode
    bool            chunked = (_protocol == "HTTP/1.1") && acceptEncodingFlag;
    Utils::t_file   file;

    if (!dataPath.empty()) {
        size_t        maxBufSize = (chunked) ? 8192 : serverMaxBodySize;
        file = Utils::readFile(dataPath, maxBufSize, 0);
        fileTime = Utils::getFileLastModTime(dataPath);
        if (!file.data) {
            std::cerr << "Response: Reading file error" << std::endl;
            // ??? throw exception
        }
        _dataBytesHandled = file.bytes_read;

        map<string, string>::const_iterator it = contentTypes.find("html");
        _contentType = (it != contentTypes.end()) ? it->second : "text/html";
    }

    makeResponseHeader(chunked, fileTime, cookies);
    setData(_header.begin().base(), _header.end().base());
    if (chunked) {
        showDebugMessage("Response: Appending chunked data");
        showDebugMessage("Response: Is EOF reached: " + to_string(file.eof));

        appendChunkedData(file.data, file.data + file.bytes_read, file.eof);
    }
    else
        appendData(file.data, file.data + file.bytes_read);
    _fullProcessed = file.eof;

    {
        showDebugMessage("Response: Response header: ");
        showDebugMessage("--------------------------------------------------");
        showDebugMessage(_header);
        showDebugMessage("--------------------------------------------------");
    }
    delete file.data;
}


// RAW DATA (FOR AUTOINDEX)
Response::Response(const string &protocol, int statusCode, bool keepAliveFlag, bool acceptEncodingFlag,
                   const char *data, size_t dataSize, string contentType, const PairList& cookies)
    : HttpMessage(protocol), _statusCode(statusCode), _contentType(contentType), _contentLength(dataSize),
    _keepAliveFlag(keepAliveFlag) {

    showDebugMessage("Response: RAW DATA constructor: code = " + to_string(statusCode));

    auto   result = global::responseStatuses.find(statusCode);
    if (result != global::responseStatuses.end()) {
        _statusMsg = result->second;
    } else {
        _statusCode = 500;
        _statusMsg = "INTERNAL SERVER ERROR";
    }

    bool     chunked = (_protocol == "HTTP/1.1") && acceptEncodingFlag;
    string   fileTime = Utils::getTimeInString();

    makeResponseHeader(chunked, fileTime, cookies);
    setData(_header.begin().base(), _header.end().base());
    if (chunked)
        appendChunkedData(data, data + dataSize, true); // TODO: Warning! Hard code 'true'
    else
        appendData(data, data + dataSize);

    {
        showDebugMessage("Response: Response header: ");
        showDebugMessage("--------------------------------------------------");
        showDebugMessage(_header);
        showDebugMessage("--------------------------------------------------");
    }
    _fullProcessed = true;
}

Response::~Response() {}

void Response::makeResponseHeader(bool HTTPv1_1, const string &fileTime,
                                  const PairList& cookies, const string &redirectURL) {

    _header.clear();
    _header.append(_protocol + " " + to_string(_statusCode) + " " + _statusMsg + "\r\n");
    _header.append("Server: " + global::APP_NAME + "\r\n");
    _header.append("Date: " + Utils::getTimeInString() + "\r\n");
    _header.append((_keepAliveFlag ? "Connection: keep-alive\r\n" : "Connection: close\r\n"));

    _header.append("Content-Type: " + _contentType + "\r\n");

    if (_statusCode == 200 || _statusCode == 201)
        _header.append("Last-Modified: " + fileTime + "\r\n");

    if (!cookies.empty()) {
        _header.append("Set-Cookie: ");
        for (auto cookie : cookies) {
            _header.append(cookie.first);
            _header.append("=");
            _header.append(cookie.second);
            _header.append("\r\n");
        }
    }

    if (HTTPv1_1) {
        _header.append("Transfer-Encoding: chunked\r\n");
        _header.append("Accept-Ranges: bytes\r\n"); // ???
    }
    else
        _header.append("Content-Length: " + to_string(_contentLength) + "\r\n");

    if (_statusCode / 100 == 3)
        _header.append("Location: " + redirectURL + "\r\n");

    _header.append("\r\n");
}

Response *Response::createResponse(const WebSession *session, const Request *request, const WebServer *server,
                                   const char *data) {

    showDebugMessage("Response: Creating response...");

    const PairList&   cookies = session->getCookiesList();

    if (!request)
        return new Response("HTTP/1.0", global::response_status::INTERNAL_ERROR,
                            false, false, cookies);

    const string&   protocol = request->getProtocol();
    bool            keepAliveFlag  = request->isKeepAlive();
    bool            acceptEncodingFlag = request->isAcceptEncoding();

    int   responseCode = request->getResponseCode();
    if (responseCode > 0)
        return new Response(protocol, responseCode, keepAliveFlag, false, cookies);

    if (!request->isFullReceived())
        return new Response(protocol, global::response_status::CONTINUE,
                            keepAliveFlag, acceptEncodingFlag, cookies);

    if (request->isHandledByCgi()) {
        if (!data)
            return new Response(protocol, global::response_status::INTERNAL_ERROR,
                                false, acceptEncodingFlag, cookies);

        const string&   status = getOptionValueFromBuffer(data, "Status");
        const string&   contentType = getOptionValueFromBuffer(data, "Content-Type");
        int             statusCode = atoi(status.c_str());

        if (statusCode == 0)
            statusCode = global::response_status::OK;

        const char*     cgiPayloadBegin = HttpMessage::findMessageEnd(data);
        const char*     cgiPayloadEnd;
        size_t          cgiPayloadSize = 0;

        if (!cgiPayloadBegin)
            return new Response(protocol, global::response_status::CONTINUE,
                                keepAliveFlag, acceptEncodingFlag, cookies);

        showDebugMessage("Response: CGI has payload");
        cgiPayloadEnd = HttpMessage::findMessageEnd(cgiPayloadBegin);
        if (cgiPayloadEnd)
            cgiPayloadSize = cgiPayloadEnd - cgiPayloadBegin;

        return new Response(protocol, statusCode, keepAliveFlag,
                            acceptEncodingFlag, cgiPayloadBegin, cgiPayloadSize, contentType, cookies);
    }

    if (request->isRedirect()) {
        showDebugMessage("Response: Creating redirect response");

        int             redirectCode = request->getRedirectCode();
        const string&   redirectURL = request->getRedirectURL();

        if (global::responseStatuses.find(redirectCode) == global::responseStatuses.end()) {
            std::cerr << "Response: No such code response" << std::endl;
            return nullptr;
        }

        return new Response(protocol, redirectCode, keepAliveFlag, acceptEncodingFlag, cookies, redirectURL);
    }

    const string&   requestMethod = request->getMethod();

    if (requestMethod == "GET")
        return createGetResponse(session, request, server);
    if (requestMethod == "POST")
        return createPostResponse(session, request, server);
    if (requestMethod == "DELETE")
        return createDeleteResponse(session, request, server);

   return new Response(protocol, global::response_status::NOT_ALLOWED,
                       keepAliveFlag, acceptEncodingFlag, cookies);
}

// Request URL:    /
// Location root:  /cshells/Desktop/git_webserv/www/default_site
// Result path:    /cshells/Desktop/git_webserv/www/default_site/


// Request URL:    /resources
// Location root:  /cshells/Desktop/git_webserv/www/default_site
// Result path:    /cshells/Desktop/git_webserv/www/default_site/resources


// Request URL:    /directory
// Location root:  /cshells/Desktop/git_webserv/www/default_site
// Result path:    /cshells/Desktop/git_webserv/www/default_site/


// Request URL:    /directory/
// Location root:  /cshells/Desktop/git_webserv/www/default_site
// Result path:    /cshells/Desktop/git_webserv/www/default_site/


// Request URL:    /directory/resources
// Location root:  /cshells/Desktop/git_webserv/www/default_site
// Result path:    /cshells/Desktop/git_webserv/www/default_site/resources


// Request URL:    /directory/resources/
// Location root:  /cshells/Desktop/git_webserv/www/default_site
// Result path:    /cshells/Desktop/git_webserv/www/default_site/resources/


// Request URL:    /directory/resources/img/image.png
// Location root:  /cshells/Desktop/git_webserv/www/default_site
// Result path:    /cshells/Desktop/git_webserv/www/default_site/resources/img/image.png

Response *Response::createGetResponse(const WebSession* session, const Request *request, const WebServer *server) {

    showDebugMessage("Response: Creating GET response...");

    const string&     protocol = request->getProtocol();
    const Location*   location = request->getLocation();
    string            requestedURL = request->getURL();
    string            requestedResourcePath = requestedURL;

    if (requestedURL != "/" && location->getURL() != "/")
        requestedResourcePath = requestedURL.substr(location->getURL().length());

    if (requestedResourcePath.empty())
        requestedResourcePath.append("/");

    string            absoluteDataPath =  location->getRoot() + requestedResourcePath;

    list<string>      indexes = location->getIndexNames();
    bool              autoindex = location->isAutoindex();
    const PairList&   cookies = session->getCookiesList();

    showDebugMessage("Response: Requested resource path: " + requestedResourcePath);
    showDebugMessage("Response: Location URL: " + location->getURL());
    showDebugMessage("Response: Location Root: " + location->getRoot());
    showDebugMessage("Response: Autoindex: " + to_string(autoindex));

    const map<string, string>&   contentTypes = server->getContentTypes();
    bool                         keepAliveFlag  = request->isKeepAlive();
    bool                         acceptEncodingFlag = request->isAcceptEncoding();
    bool                         pathExistence, pathAccess;

    if (requestedResourcePath == "/") {
        showDebugMessage("Response: Index is required");

        for (auto indexFileName = indexes.begin(); indexFileName != indexes.end(); ++indexFileName) {
            showDebugMessage("Response: Searching " + *indexFileName + " in " + absoluteDataPath);
            pathExistence = Utils::isPathExist(absoluteDataPath + *indexFileName);
            showDebugMessage("Response: Index existing: " + to_string(pathExistence));
            if (pathExistence) {
                absoluteDataPath += *indexFileName;
                break;
            }
        }
    }

    showDebugMessage("Response: Path required: " + absoluteDataPath);
    pathExistence = Utils::isPathExist(absoluteDataPath);
    showDebugMessage("Response: Path existing: " + to_string(pathExistence));
    if (!pathExistence) {
        return new Response(protocol, global::response_status::NOT_FOUND,
                            keepAliveFlag, acceptEncodingFlag, cookies);
    }

    pathAccess = (access(absoluteDataPath.c_str(), R_OK) == 0);
    showDebugMessage("Response: Path access: " + to_string(pathAccess));
    if (!pathAccess) {
        return new Response(protocol, global::response_status::FORBIDDEN,
                            keepAliveFlag, acceptEncodingFlag, cookies);
    }

    if (*absoluteDataPath.rbegin() == '/') {
        showDebugMessage("Response: Handling as directory...");

        if (!autoindex) {
            return new Response(protocol, global::response_status::FORBIDDEN,
                                keepAliveFlag, acceptEncodingFlag, cookies);
        }
        string   autoindexPage = makeAutoindexPage(requestedURL, absoluteDataPath);
        size_t   autoindexSize = autoindexPage.length();

        return new Response(protocol, global::response_status::OK,
                            keepAliveFlag, acceptEncodingFlag, autoindexPage.c_str(),
                            autoindexSize, "text/html", cookies);
    }

    size_t   bytesAlreadySent = request->getHandledDataSize();
    if (bytesAlreadySent > 0)
        return new Response(protocol, absoluteDataPath, bytesAlreadySent); // TODO: ??? what's logic, need to check this out

    return new Response(protocol, global::response_status::OK,
                        keepAliveFlag, acceptEncodingFlag, absoluteDataPath, contentTypes, cookies);
}

Response *Response::createPostResponse(const WebSession* session, const Request* request, const WebServer* server) {

    (void)server;

    showDebugMessage("Response: Creating POST response...");

    struct stat       fi;
    const string&     dataPath = request->getDataPath();
    const string&     protocol = request->getProtocol();
    bool              keepAliveFlag  = request->isKeepAlive();
    bool              acceptEncodingFlag = request->isAcceptEncoding();
    const PairList&   cookies = session->getCookiesList();

    bzero(&fi, sizeof(fi));
    stat(dataPath.c_str(), &fi);

    showDebugMessage("Response: Saved file size: " + to_string(fi.st_size));
    showDebugMessage("Response: Content-Length: " + to_string(request->getContentLength()));

    if (fi.st_size != request->getContentLength()) {
        return new Response(protocol, global::response_status::BAD_REQUEST,
                            keepAliveFlag, acceptEncodingFlag, cookies);
    }

    return new Response(protocol, global::response_status::CREATED,
                        keepAliveFlag, acceptEncodingFlag, cookies);
}

Response *Response::createDeleteResponse(const WebSession* session, const Request* request, const WebServer* server) {

    (void)server;

    showDebugMessage("Response: Creating DELETE response...");

    const string&                protocol = request->getProtocol();
    bool                         keepAliveFlag  = request->isKeepAlive();
    bool                         acceptEncodingFlag = request->isAcceptEncoding();
    const string&                url = request->getURL();
    const Location*              location = request->getLocation();
    const PairList&              cookies = session->getCookiesList();

    size_t   lastSlashPosition = url.rfind('/');
    if (lastSlashPosition == string::npos)
        return new Response(protocol, global::response_status::INTERNAL_ERROR,
                            keepAliveFlag, acceptEncodingFlag, cookies);

    string   absoluteDataPath =  location->getRoot() + url.substr(lastSlashPosition);
    showDebugMessage("Response: Absolute path to file: " + absoluteDataPath);

    bool   pathExist = !access(absoluteDataPath.c_str(), F_OK);
    if (!pathExist)
        return new Response(protocol, global::response_status::NOT_FOUND,
                            keepAliveFlag, acceptEncodingFlag,  cookies);

    bool   accessRights = !access(absoluteDataPath.c_str(), W_OK);
    if (!accessRights)
        return new Response(protocol, global::response_status::FORBIDDEN,
                            keepAliveFlag, acceptEncodingFlag, cookies);

    bool   removeOk = !remove(absoluteDataPath.c_str());
    if (!removeOk)
        return new Response(protocol, global::response_status::INTERNAL_ERROR,
                            keepAliveFlag, acceptEncodingFlag, cookies);

    return new Response(protocol, global::response_status::OK,
                        keepAliveFlag, acceptEncodingFlag, cookies);
}

string Response::makeStatusPage() {
    showDebugMessage("Response: Preparing status page...");

    return "<!DOCTYPE html>\n"
           "<html>\n"
           "<head>\n"
           "    <title>" + to_string(_statusCode) + " " + " " + _statusMsg + "</title>\n"
           "</head>\n"
           "<body>\n"
           "<p style=\"text-align:center\"><span style=\"font-size:72px\"><strong>" + to_string(_statusCode) + " " + _statusMsg + "</strong></span></p>\n"
           "\n"
           "<p style=\"text-align:center\">" + global::APP_NAME + "</p>\n"
           "</body>\n"
           "</html>";
}

string Response::makeAutoindexPage(const string &url, const string &abs_path) {

    showDebugMessage("Response: Preparing autoindex page...");

    string   res = "<html>\n"
                   "<head><title>Index of ";


    res += url;
    res += "</title></head>\n"
            "<body bgcolor=\"white\">\n"
            "<h1>Index of ";
    res += url;
    res += "</h1><hr><pre><a href=\"../\">../</a>\n";
    list<struct dirent>  content;
    Utils::getDirContent(abs_path.c_str(), content);
    for (auto it = content.begin(); it != content.end(); ++it) {
        if (!strcmp(it->d_name, ".") || !strcmp(it->d_name, ".."))
            continue;
        res += makeAutoindexLine(it, url, abs_path);
    }
    res += "</pre><hr></body>\n"
            "</html>";

    return res;
}

string Response::makeAutoindexLine(list<struct dirent>::iterator file, const string& url, const string& path) {
    struct stat   fi;
    string        result = "<a href=\"";
    string        size;
    const char*   tabs = "\t\t\t\t\t\t\t\t\t";
    string        name = file->d_name;
    string        currentPath = path + file->d_name;

    stat(currentPath.c_str(), &fi);
    if (file->d_type == DT_DIR) {
        name += "/";
        size = "-";
    } else {
        size = to_string(fi.st_size);

    }
    size_t   tabOffset = name.length() / 8;
    if (tabOffset >= strlen(tabs))
        tabOffset = strlen(tabs) - 1;

    result.append(url);
    if (*url.rbegin() != '/')
        result.append("/");

    result.append(name);
    result.append("\">");
    result.append(name);
    result.append("</a>");
    result.append(&tabs[tabOffset]);
    result.append(Utils::getFileLastModTime(name));
    result.append("\t\t\t");
    result.append(size);
    result.append("\n");

    return result;
}

const string &Response::getHeader() const { return _header; }

size_t Response::getHandledDataSize() const { return _dataBytesHandled; }

bool Response::isFullProcessed() const { return _fullProcessed; }