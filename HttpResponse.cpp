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

HttpResponse::HttpResponse(int status_code, string status_msg)
: HttpMessage(), _status_code(status_code), _status_msg(status_msg) {
    makeResponseHeader("");
}

HttpResponse::HttpResponse(int status_code, string status_msg, string data_path, const map<string, string> &content_types)
: HttpMessage(), _status_code(status_code), _status_msg(status_msg), _data_path(data_path) {

    string              file_time;
    Utils::t_file       file;

    if (!data_path.empty()) {
        file_time = Utils::getFileLastModTime(data_path);
        file = Utils::readFile(data_path);
        // TODO: read not full file - chunked ( seekg() )
        if (!file.data) {
            std::cout << "Response: Reading file error" << std::endl;
        }
        setData(file.data, file.data + file.size);
        _content_type = Utils::findKey(content_types.begin(), content_types.end(), file.type)->first;
    }
    makeResponseHeader(file_time);
}

HttpResponse::HttpResponse(int status_code, string status_msg, const char *data, size_t data_size, string content_type)
: HttpMessage(), _status_code(status_code), _status_msg(status_msg), _content_type(content_type) {

    string              file_time = Utils::getTime();

    setData(data, data + data_size);
    makeResponseHeader(file_time);
}

HttpResponse::~HttpResponse() {}

void HttpResponse::makeResponseHeader(const string &file_time) {
    _header.clear();
    _header.append("HTTP/1.1 " + to_string(_status_code) + "\n");
    _header.append("Date: " + Utils::getTime() + "\n");
    _header.append("Server: " + string("webserv") + "\n");  // TODO: serv name
    if (_status_code == 200 || _status_code == 201)
        _header.append("Last-Modified: " + file_time + "\n");
    _header.append("Accept-Ranges: bytes\n");
    _header.append("Content-Length: " + std::to_string(_content_length) + "\n");
    if (!_content_type.empty())
        _header.append("Content-Type: " + _content_type + "\n\n");
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

HttpResponse *HttpResponse::createGetResponse(HttpRequest *request, WebServer *server) {

    string                      path = "." + request->getPath();
    bool                        path_exist, path_access;
    const list<string>&         index_list = server->getIndexList();
    const map<string, string>   content_types = server->getContentTypes();
    bool                        _autoindex = false;  // TODO: autoindex value

    if (path == "/") {

        for (std::list<std::string>::const_iterator it = index_list.begin(); it != index_list.end(); ++it) {
            if ((path_exist = Utils::isPathExist(*it))) {
                path += *it;
                break;
            }
        }
    }
    path_exist = Utils::isPathExist(path);
    if (!path_exist) {
        return new HttpResponse(404, "NOT FOUND", "error_pages/404.html", content_types);
    }
    path_access = access(path.c_str(), R_OK) == 0;
    if (!path_access) {
        return new HttpResponse(403, "FORBIDDEN", "error_pages/403.html", content_types);
    }
    if (path[path.length() - 1] == '/') {
        if (!_autoindex) {
            return new HttpResponse(403, "FORBIDDEN", "error_pages/403.html", content_types);
        }
        string autoindex_page = makeAutoindexPage("./");
        size_t autoindex_size = autoindex_page.size();

        return new HttpResponse(200, "OK", autoindex_page.c_str(), autoindex_size);
    }
    return new HttpResponse(200, "OK", path, content_types);
}

HttpResponse *HttpResponse::createPostResponse(HttpRequest *request, WebServer *server) {
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



    bool                        path_exist, remove_ok;
    std::string                 path = "." + request->getUri();

    std::cout << "Path DELETE requested: " << path << std::endl;
    path_exist = !access(path.c_str(), F_OK);
    if (!path_exist)
        return new HttpResponse(404, "NOT FOUND",
                                "./error_pages/404.html", server->getContentTypes());
    remove_ok = !remove(path.c_str());
    if (!remove_ok)
        return new HttpResponse(500, "INTERNAL SERVER ERROR",
                                "./error_pages/500.html", server->getContentTypes());

    return new HttpResponse(200, "OK", "./deleted.html", server->getContentTypes());
}

//HttpResponse *HttpResponse::createErrorResponse(int status_code, string status_msg) {}

string HttpResponse::makeAutoindexPage(string path) {
    std::string res = "<html>\n"
                       "<head><title>Index of ";


    res += &path[1];
    res += "</title></head>\n"
            "<body bgcolor=\"white\">\n"
            "<h1>Index of ";
    res += &path[1];
    res += "</h1><hr><pre><a href=\"../\">../</a>\n";
    std::list<struct dirent>  content;
    Utils::getDirContent(path.c_str(), content);
    for (std::list<struct dirent>::iterator it = content.begin(); it != content.end(); ++it) {
        if (!strcmp(it->d_name, ".") || !strcmp(it->d_name, ".."))
            continue;
        res += makeAutoindexLine(it, path);
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