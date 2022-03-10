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

HttpResponse::HttpResponse(int status_code, const string &status_msg)
    : HttpMessage(), _status_code(status_code), _status_msg(status_msg) {
    makeResponseHeader("");
}

HttpResponse::HttpResponse(int status_code, const string &status_msg, const string &connection,
                           const string &data_path, const map<string, string> &content_types)
    : HttpMessage(), _status_code(status_code), _status_msg(status_msg), _connection(connection) {

    string              file_time;
    Utils::t_file       file;

    if (!data_path.empty()) {
        file_time = Utils::getFileLastModTime(data_path);
        file = Utils::readFile(data_path);
        // TODO: read not full file - chunked ( seekg() )
        if (!file.data) {
            std::cout << "HttpResponse: Reading file error" << std::endl;
        }
        setData(file.data, file.data + file.size);
//        std::cout << "FILE TYPE: " << file.type << std::endl;
//        std::cout << "KEYS: " << std::endl;
//        for (map<string, string>::const_iterator it = content_types.begin(); it != content_types.cend(); ++it) {
//            std::cout << it->first << std::endl;
//        }
        map<string, string>::const_iterator it = content_types.find("html");
        if (it != content_types.end())
            _content_type = it->second;
        else
            _content_type = "text/html";

    }
    makeResponseHeader(file_time);
    std::cout << "HttpResponse: Response header: " << std::endl;
    std::cout << "--------------------------------------------------" << std::endl;
    std::cout << _header << std::endl;
    std::cout << "--------------------------------------------------" << std::endl;
}

HttpResponse::HttpResponse(int status_code, const string &status_msg, const string &connection,
                           const char *data, size_t data_size, string content_type)
    : HttpMessage(), _status_code(status_code), _status_msg(status_msg), _connection(connection), _content_type(content_type) {

    string              file_time = Utils::getTime();

    setData(data, data + data_size);
    makeResponseHeader(file_time);
    std::cout << "HttpResponse: Response header: " << std::endl;
    std::cout << "--------------------------------------------------" << std::endl;
    std::cout << _header << std::endl;
    std::cout << "--------------------------------------------------" << std::endl;
}

HttpResponse::~HttpResponse() {}

void HttpResponse::makeResponseHeader(const string &file_time) {
    _header.clear();
    _header.append("HTTP/1.1 " + to_string(_status_code) + " " + _status_msg + "\n");
    _header.append("Server: " + string("webserv") + "\n");  // TODO: serv name
    _header.append("Date: " + Utils::getTime() + "\n");
    if (!_content_type.empty())
        _header.append("Content-Type: " + _content_type + "\n");
    _header.append("Connection: " + _connection + "\n");
    if (_status_code == 200 || _status_code == 201)
        _header.append("Last-Modified: " + file_time + "\n");
    _header.append("Accept-Ranges: bytes\n");
    _header.append("Content-Length: " + to_string(_data.size()) + "\n\n");
}

HttpResponse *HttpResponse::createResponse(HttpRequest *request, WebServer *server) {

    if (!request->getFullReceivedFlag())
        return new HttpResponse(100, "CONTINUE");

    const string    &request_type = request->getType();

    if (request_type == "GET")
        return createGetResponse(request, server);
    if (request_type == "POST")
        return createPostResponse(request, server);
    if (request_type == "DELETE")
        return createDeleteResponse(request, server);

   return new HttpResponse(405, "NOT ALLOWED");
}

//void    definePath(string& result_path, const string& requested_path, const list<LocationTraits>& location_list) {
//    for (list<LocationTraits>::const_iterator it = location_list.cbegin(); it != location_list.cend(); ++it) {
//        if ((*it) == requested_path)
//    }
//};

HttpResponse *HttpResponse::createGetResponse(HttpRequest *request, WebServer *server) {

    std::cout << "HttpResponse: Creating GET response..." << std::endl;

    bool                        path_exist, path_access;

    const map<string, string>&  content_types = server->getContentTypes();
    const string                connection = request->isKeepAlive() ? "keep-alive" : "close";


    string                          abs_path;
    string                          root;
    list<string>                    index_list;
    bool                            autoindex;
    const string                    &requested_path = request->getPath();

    std::cout << "HttpResponse: Defining location..." << std::endl;
    const WebServer::ServerTraits   *server_traits = server->getLocationTraits(requested_path);
    const WebServer::LocationTraits *location_traits = dynamic_cast<const WebServer::LocationTraits*>(server_traits);
    std::cout << "HttpResponse: Defining location is done" << std::endl;
    if (location_traits) {
//        root = location_traits->getRoot();
        abs_path = location_traits->getRoot() + requested_path;
        index_list = location_traits->getIndexList();
        autoindex = location_traits->isAutoindex();
    } else {
//        root = server_traits->getRoot();
        abs_path =  server_traits->getRoot() + requested_path;
        index_list = server_traits->getIndexList();
        autoindex = server_traits->isAutoindex();
    }

//    std::cout << "PATH! :" << path << std::endl;

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
    std::cout << "HttpResponse: Path required: " << abs_path << std::endl;
    path_exist = Utils::isPathExist(abs_path);
    std::cout << "HttpResponse: Path existing: " << path_exist << std::endl;
    if (!path_exist) {
        return new HttpResponse(404, "NOT FOUND", connection, "error_pages/404.html", content_types);
    }
    path_access = (access(abs_path.c_str(), R_OK) == 0);
    std::cout << "HttpResponse: Path access: " << path_access << std::endl;
    if (!path_access) {
        return new HttpResponse(403, "FORBIDDEN", connection, "error_pages/403.html", content_types);
    }
    if (abs_path[abs_path.length() - 1] == '/') {
        std::cout << "HttpResponse: Handling as directory..." << std::endl;

        if (!autoindex) {
            return new HttpResponse(403, "FORBIDDEN", connection, "error_pages/403.html", content_types);
        }
        string autoindex_page = makeAutoindexPage(requested_path, abs_path);
        size_t autoindex_size = autoindex_page.size();

        return new HttpResponse(200, "OK", connection, autoindex_page.c_str(), autoindex_size);
    }
    return new HttpResponse(200, "OK", connection, abs_path, content_types);
}

HttpResponse *HttpResponse::createPostResponse(HttpRequest *request, WebServer *server) {
    std::cout << "HttpResponse: Creating POST response..." << std::endl;

    struct stat fi;
    const string &uri = request->getUri();

    bzero(&fi, sizeof(fi));
    stat(uri.c_str(), &fi);
    if (fi.st_size != request->getContentLength()) {
        return new HttpResponse(40012, "ERROR");
    }

    return new HttpResponse(201, "CREATED");
}

HttpResponse *HttpResponse::createDeleteResponse(HttpRequest *request, WebServer *server) {
    std::cout << "HttpResponse: Creating DELETE response..." << std::endl;

    bool                        path_exist, remove_ok;
    std::string                 path = "." + request->getUri();
    const string                connection = request->isKeepAlive() ? "keep-alive" : "close";

    std::cout << "Path DELETE requested: " << path << std::endl;
    path_exist = !access(path.c_str(), F_OK);
    if (!path_exist)
        return new HttpResponse(404, "NOT FOUND", connection,
                                "./error_pages/404.html", server->getContentTypes());
    remove_ok = !remove(path.c_str());
    if (!remove_ok)
        return new HttpResponse(500, "INTERNAL SERVER ERROR", connection,
                                "./error_pages/500.html", server->getContentTypes());

    return new HttpResponse(200, "OK", "./deleted.html", connection, server->getContentTypes());
}

//HttpResponse *HttpResponse::createErrorResponse(int status_code, string status_msg) {}

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