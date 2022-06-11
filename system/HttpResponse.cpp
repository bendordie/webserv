/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cshells <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/31 19:40:20 by cshells           #+#    #+#             */
/*   Updated: 2022/01/31 19:40:21 by cshells          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponse.hpp"

// CHUNKED RESPONSE
HttpResponse::HttpResponse(const string &protocol, const string &data_path, size_t data_bytes_processed)
    : HttpMessage(protocol), _dataBytesHandled(data_bytes_processed), _contentType("text/html") {

    std::cout << "HttpResponse: First constructor (CHUNKED): code = " << _statusCode << std::endl;

    if (data_path.empty()) {
        // ???
    }
    Utils::t_file file = Utils::readFile(data_path, 65535,data_bytes_processed); // TODO: func for calculating suitable chunk size
    if (!file.data) {
        std::cout << "HttpResponse: Reading file error" << std::endl;
        // ???
    }

    setChunkedData(file.data, file.data + file.bytes_read, file.eof);
    _dataBytesHandled += file.bytes_read;
    _fullProcessed = file.eof;
}

// SIMPLE RESPONSE
HttpResponse::HttpResponse(const string &protocol, Status status, const string &serverApplicationName)
    : HttpMessage(protocol), _contentType("text/html") {

    std::cout << "HttpResponse: Second constructor (SIMPLE)" << std::endl;

    bool           isHTTPv1_1 = (_protocol == "HTTP/1.1");
    const string   &currentTime = Utils::getTimeInString();
    makeResponseHeader(isHTTPv1_1, currentTime, serverApplicationName);
    setData(_header.begin().base(), _header.end().base());

    const string   &page = makeStatusPage(static_cast<int>(status), serverApplicationName);
    appendData(page.begin().base(), page.end().base());
    _data_size = page.size();
    _fullProcessed = true;
}

// FILE RESPONSE
HttpResponse::HttpResponse(const string &protocol, int status_code, const string &status_msg, const string &connection,
                           const string &data_path, const map<string, string> &content_types)
    : HttpMessage(protocol), _statusCode(status_code), _statusMsg(status_msg), _connection(connection),
    _dataBytesHandled(0), _dataBytesRemaining(0), _fullProcessed(false) {

    std::cout << "HttpResponse: Third constructor (FILE): code = " << status_code << std::endl;

    string          file_time;
    long            client_max_body_size = 100000; // TODO: hardcode
    bool            is_HTTPv1_1 = (_protocol == "HTTP/1.1");
    Utils::t_file   file = { 0, 0 ,"" };

    if (!data_path.empty()) {
        size_t        max_buf_size = (is_HTTPv1_1) ? 65535 : client_max_body_size;
        file = Utils::readFile(data_path, max_buf_size);
        file_time = Utils::getFileLastModTime(data_path);
        if (!file.data) {
            std::cout << "HttpResponse: Reading file error" << std::endl;
            // ???
        }
        _dataBytesHandled = file.bytes_read;

        map<string, string>::const_iterator it = content_types.find("html");
        _contentType = (it != content_types.end()) ? it->second : "text/html";
    }
    makeResponseHeader(is_HTTPv1_1, file_time);
    setData(_header.begin().base(), _header.end().base());
    if (file.data && file.bytes_read > 0) {
        if (is_HTTPv1_1) {
            std::cout << "Append chunked" << std::endl;
            std::cout << "IsEOF? " << file.eof << std::endl;
            appendChunkedData(file.data, file.data + file.bytes_read, file.eof);
        }
        else
            appendData(file.data, file.data + file.bytes_read);
    }
    _fullProcessed = file.eof;

    std::cout << "HttpResponse: Response header: " << std::endl;
    std::cout << "--------------------------------------------------" << std::endl;
    std::cout << _header << std::endl;
    std::cout << "--------------------------------------------------" << std::endl;
}


// RAW DATA RESPONSE (FOR AUTOINDEX)
HttpResponse::HttpResponse(const string &protocol, int status_code, const string &status_msg, const string &connection,
                           const char *data, size_t data_size, string content_type)
    : HttpMessage(protocol), _statusCode(status_code), _statusMsg(status_msg),
    _connection(connection), _contentType(content_type) {

    std::cout << "HttpResponse: Forth constructor (AUTOINDEX): code = " << status_code << std::endl;

    bool     is_HTTPv1_1 = (_protocol == "HTTP/1.1");
    string   file_time = Utils::getTimeInString();

    makeResponseHeader(is_HTTPv1_1, file_time);
    setData(_header.begin().base(), _header.end().base());
    if (is_HTTPv1_1)
        appendChunkedData(data, data + data_size, true); // TODO: Warning! Hard code 'true'
    else
        appendData(data, data + data_size);

    std::cout << "HttpResponse: Response header: " << std::endl;
    std::cout << "--------------------------------------------------" << std::endl;
    std::cout << _header << std::endl;
    std::cout << "--------------------------------------------------" << std::endl;
}

HttpResponse::~HttpResponse() {}

void HttpResponse::makeResponseHeader(bool is_HTTPv1_1, const string &fileTime, const string &serverName) {
    _header.clear();
    _header.append(_protocol + " " + to_string(_statusCode) + " " + _statusMsg + "\r\n");
//    if (_statusCode / 100 == 3) {
////        _header.append("Location: " + _redirect);
//        _header.append("Location: https://vk.com\r\n\r\n");
//        return;
//    }
    _header.append("Server: " + serverName + "\r\n");
    _header.append("Date: " + Utils::getTimeInString() + "\r\n");
    _header.append("Content-Type: " + _contentType + "\r\n");  // TODO: default content type (html) in all constructors
    _header.append("Connection: " + _connection + "\r\n");
    if (_statusCode == 200 || _statusCode == 201)
        _header.append("Last-Modified: " + fileTime + "\r\n");
    _header.append("Accept-Ranges: bytes\r\n"); // ???
    if (is_HTTPv1_1)
        _header.append("Transfer-Encoding: chunked\r\n\r\n");
    else
        _header.append("Content-Length: " + to_string(_data_size) + "\r\n\r\n"); // TODO: what size if we got simple response?
}

HttpResponse *HttpResponse::createResponse(HttpRequest *request, WebServer *server) {

    std::cout << "HttpResponse: Trying to create response" << std::endl;

    const string             &protocol = request->getProtocol();
    const map<int, string>   &statuses = server->getResponseStatuses();
    const string             &serverApplicationName = server->getName();

    if (!request->isFullReceived())
        return new HttpResponse(protocol, Status::CONTINUE, serverApplicationName);
    if (request->isRedirect())
        return new HttpResponse(protocol, Status::MOVED_PERMANENTLY, serverApplicationName);


    const string   &request_type = request->getType();

    if (request_type == "GET")
        return createGetResponse(request, server);
    if (request_type == "POST")
        return createPostResponse(request, server);
    if (request_type == "DELETE")
        return createDeleteResponse(request, server);

   return new HttpResponse(protocol, Status::NOT_ALLOWED, serverApplicationName);
}

HttpResponse *HttpResponse::createGetResponse(HttpRequest *request, WebServer *server) {

    std::cout << "HttpResponse: Creating GET response..." << std::endl;

    const string          &protocol = request->getProtocol();
    const string          &requested_path = request->getUri();
    const string          hostname = request->getHeaderEntryValue("Host");
    const ServerOptions   *server_traits = server->getLocationOptions(hostname, requested_path);
    string                abs_path =  server_traits->getRoot() + requested_path;
    list<string>          index_list = server_traits->getIndexList();
    bool                  autoindex = server_traits->isAutoindex();
//    string              server_display_name = server->getDisplayName();

    std::cout << "HttpResponse: Requested path: " << requested_path << std::endl;
    std::cout << "HttpResponse: Defining location..." << std::endl;


    std::cout << "HttpResponse: Autoindex: " << autoindex << std::endl;

//    std::cout << "PATH! :" << path << std::endl;

    bool path_exist, path_access;
    if (requested_path == "/") {
        std::cout << "HttpResponse: Index is required" << std::endl;

        for (std::list<std::string>::const_iterator it = index_list.begin(); it != index_list.end(); ++it) {
            std::cout << "HttpResponse: Searching " << *it << " in " << abs_path << std::endl;
            path_exist = Utils::isPathExist(abs_path + *it);
            std::cout << "HttpResponse: Index existing: " << path_exist << std::endl;
            if (path_exist) {
                abs_path += *it;
                break;
            }
        }
    }
    const map<string, string>&  content_types = server->getContentTypes();
    const string                connection = request->isKeepAlive() ? "keep-alive" : "close";
    std::cout << "HttpResponse: Path required: " << abs_path << std::endl;
    path_exist = Utils::isPathExist(abs_path);
    std::cout << "HttpResponse: Path existing: " << path_exist << std::endl;
    if (!path_exist) {
        return new HttpResponse(protocol, 404, "NOT FOUND",
                                connection, "error_pages/404.html", content_types);
    }
    path_access = (access(abs_path.c_str(), R_OK) == 0);
    std::cout << "HttpResponse: Path access: " << path_access << std::endl;
    if (!path_access) {
        return new HttpResponse(protocol, 403, "FORBIDDEN",
                                connection, "error_pages/403.html", content_types);
    }
    if (abs_path[abs_path.length() - 1] == '/') {
        std::cout << "HttpResponse: Handling as directory..." << std::endl;

        if (!autoindex) {
            return new HttpResponse(protocol, 403, "FORBIDDEN",
                                    connection, "error_pages/403.html", content_types);
        }
        string autoindex_page = makeAutoindexPage(requested_path, abs_path);
        size_t autoindex_size = autoindex_page.length();

        return new HttpResponse(protocol, 200, "OK",
                                connection, autoindex_page.c_str(), autoindex_size, "text/html");
    }
    size_t   bytes_already_sent = request->getHandledDataSize();
    if (bytes_already_sent > 0)
        return new HttpResponse(protocol, abs_path, bytes_already_sent);

    return new HttpResponse(protocol, 200, "OK",
                            connection, abs_path, content_types);
}

HttpResponse *HttpResponse::createPostResponse(HttpRequest *request, WebServer *server) {
    std::cout << "HttpResponse: Creating POST response..." << std::endl;

    struct stat fi;
    const string &data_path = request->getDataPath();
    const string &protocol = request->getProtocol();

    bzero(&fi, sizeof(fi));
    stat(data_path.c_str(), &fi);
    if (fi.st_size != request->getContentLength()) {
        return new HttpResponse(protocol, 40012, "ERROR");
    }

    return new HttpResponse(protocol, 201, "CREATED");
}

HttpResponse *HttpResponse::createDeleteResponse(HttpRequest *request, WebServer *server) {
    std::cout << "HttpResponse: Creating DELETE response..." << std::endl;

    const string              &protocol = request->getProtocol();
    std::string               data_path = "." + request->getDataPath();
    const string              connection = request->isKeepAlive() ? "keep-alive" : "close";
    const map<string, string> &content_types = server->getContentTypes();

    std::cout << "Path DELETE requested: " << data_path << std::endl;
    bool path_exist = !access(data_path.c_str(), F_OK);
    if (!path_exist)
        return new HttpResponse(protocol, 404, "NOT FOUND", connection,
                                "./error_pages/404.html", content_types);
    bool remove_ok = !remove(data_path.c_str());
    if (!remove_ok)
        return new HttpResponse(protocol, 500, "INTERNAL SERVER ERROR", connection,
                                "./error_pages/500.html", content_types);

    return new HttpResponse(protocol, 200, "OK", connection,
                            "./deleted.html", content_types);
}

string HttpResponse::makeStatusPage(int code, const string &serverName) {
    std::cout << "HttpResponse: Preparing status page..." << std::endl;

    auto res = _status.find(code);
    if (res == _status.end()) {
        return "<!DOCTYPE html>\n"
               "<html>\n"
               "<head>\n"
               "    <title>400 Bad Request</title>\n"
               "</head>\n"
               "<body>\n"
               "<p style=\"text-align:center\"><span style=\"font-size:72px\"><strong>400 Bad Request</strong></span></p>\n"
               "\n"
               "<p style=\"text-align:center\">" + serverName + "</p>\n"
               "</body>\n"
               "</html>";
    }

    return "<!DOCTYPE html>\n"
           "<html>\n"
           "<head>\n"
           "    <title>" + to_string(code) + " " + " " + res->second + "</title>\n"
           "</head>\n"
           "<body>\n"
           "<p style=\"text-align:center\"><span style=\"font-size:72px\"><strong>400 Bad Request</strong></span></p>\n"
           "\n"
           "<p style=\"text-align:center\">" + serverName + "</p>\n"
           "</body>\n"
           "</html>";
}

string HttpResponse::makeAutoindexPage(const string &uri, const string &abs_path) {
    std::cout << "HttpResponse: Preparing autoindex page..." << std::endl;

    std::string res = "<html>\n"
                       "<head><title>Index of ";


    res += uri;
    res += "</title></head>\n"
            "<body bgcolor=\"white\">\n"
            "<h1>Index of ";
    res += uri;
    res += "</h1><hr><pre><a href=\"../\">../</a>\n";
    std::list<struct dirent>  content;
    Utils::getDirContent(abs_path.c_str(), content);
    for (std::list<struct dirent>::iterator it = content.begin(); it != content.end(); ++it) {
        if (!strcmp(it->d_name, ".") || !strcmp(it->d_name, ".."))
            continue;
        res += makeAutoindexLine(it, abs_path);
    }
    res += "</pre><hr></body>\n"
            "</html>";

    return res;
}

string HttpResponse::makeAutoindexLine(list<struct dirent>::iterator file, const string &path) {
    struct stat fi;
    std::string res = "<a href=\"";
    std::string size;
    const char* tabs = "\t\t\t\t\t\t\t\t\t";
    std::string name = file->d_name;
    std::string curr_path = path + file->d_name;
    int         tab_offset = strlen(file->d_name) / 8;

    stat(curr_path.c_str(), &fi);
    if (file->d_type == DT_DIR) {
        name += "/";
        size = "-";
    } else {
        size = std::to_string(fi.st_size);
    }
    if (tab_offset >= strlen(tabs))
        tab_offset = strlen(tabs) - 1;
    res += name + "\">" + name + "</a>" + &tabs[tab_offset] + Utils::getFileLastModTime(name) + "\t\t\t" + size + "\n";

    return res;
}

const string &HttpResponse::getHeader() const { return _header; }

bool HttpResponse::isFullProcessed() const { return _fullProcessed; }