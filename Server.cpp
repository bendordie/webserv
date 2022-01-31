/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cshells <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/16 17:50:28 by cshells           #+#    #+#             */
/*   Updated: 2021/12/16 17:50:29 by cshells          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

//Server::Server() {}

Server::Server(int port, int backlog)
:   _socket(-1), _max_fd(0), _port(port), _backlog(backlog), _response_data(NULL),
    _response_data_size(0), _response_full_size(0), _autoindex(true), _log_data(NULL), _response_header(""),
    _name("webserv"), _max_body_size(MAX_BODY) {
    initContentTypes();
    initIndexPages();
}

Server::~Server() {
    std::cout << "Shutting down the server..." << std::endl;
    for (std::list<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        delete *it;
    }
    close(_socket);
    sleep(DEBUG_DELAY_SEC);
    std::cout << "Exit" << std::endl;
}

void Server::error(std::string err_str) {
    std::cout << "Error: " << err_str << std::endl;
    std::cout << "Shutting down the server..." << std::endl;
    for (std::list<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        delete *it;
    }
    close(_socket);
    sleep(DEBUG_DELAY_SEC);
    std::cout << "Exit" << std::endl;
    exit(1);
}

void Server::initContentTypes() {
    _content_type.insert(std::make_pair("aac", "audio/aac"));
    _content_type.insert(std::make_pair("abw", "application/x-abiword"));
    _content_type.insert(std::make_pair("arc", "application/x-freearc"));
    _content_type.insert(std::make_pair("avi", "video/x-msvideo"));
    _content_type.insert(std::make_pair("azw", "application/vnd.amazon.ebook"));
    _content_type.insert(std::make_pair("bin", "application/octet-stream"));
    _content_type.insert(std::make_pair("bmp", "image/bmp"));
    _content_type.insert(std::make_pair("bz", "application/x-bzip"));
    _content_type.insert(std::make_pair("bz2", "application/x-bzip2"));
    _content_type.insert(std::make_pair("csh", "application/x-csh"));
    _content_type.insert(std::make_pair("css", "text/css"));
    _content_type.insert(std::make_pair("csv", "text/csv"));
    _content_type.insert(std::make_pair("doc", "application/msword"));
    _content_type.insert(std::make_pair("docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document"));
    _content_type.insert(std::make_pair("eot", "application/vnd.ms-fontobject"));
    _content_type.insert(std::make_pair("epub", "application/epub+zip"));
    _content_type.insert(std::make_pair("gz", "application/gzip"));
    _content_type.insert(std::make_pair("gif", "image/gif"));
    _content_type.insert(std::make_pair("htm", "text/html"));
    _content_type.insert(std::make_pair("html", "text/html"));
    _content_type.insert(std::make_pair("ico", "image/vnd.microsoft.icon"));
    _content_type.insert(std::make_pair("ics", "text/calendar"));
    _content_type.insert(std::make_pair("jar", "application/java-archive"));
    _content_type.insert(std::make_pair("jpeg", "image/jpeg"));
    _content_type.insert(std::make_pair("jpg", "image/jpeg"));
    _content_type.insert(std::make_pair("js", "text/javascript"));
    _content_type.insert(std::make_pair("json", "application/json"));
    _content_type.insert(std::make_pair("jsonld", "application/ld+json"));
    _content_type.insert(std::make_pair("mid", "audio/midi audio/x-midi"));
    _content_type.insert(std::make_pair("midi", "audio/midi audio/x-midi"));
    _content_type.insert(std::make_pair("mjs", "text/javascript"));
    _content_type.insert(std::make_pair("mp3", "audio/mpeg"));
    _content_type.insert(std::make_pair("mpeg", "video/mpeg"));
    _content_type.insert(std::make_pair("mpkg", "application/vnd.apple.installer+xml"));
    _content_type.insert(std::make_pair("odp", "application/vnd.oasis.opendocument.presentation"));
    _content_type.insert(std::make_pair("ods", "application/vnd.oasis.opendocument.spreadsheet"));
    _content_type.insert(std::make_pair("odt", "application/vnd.oasis.opendocument.text"));
    _content_type.insert(std::make_pair("oga", "audio/ogg"));
    _content_type.insert(std::make_pair("ogv", "video/ogg"));
    _content_type.insert(std::make_pair("ogx", "application/ogg"));
    _content_type.insert(std::make_pair("opus", "audio/opus"));
    _content_type.insert(std::make_pair("otf", "font/otf"));
    _content_type.insert(std::make_pair("png", "image/png"));
    _content_type.insert(std::make_pair("pdf", "application/pdf"));
    _content_type.insert(std::make_pair("php", "application/php"));
    _content_type.insert(std::make_pair("ppt", "application/vnd.ms-powerpoint"));
    _content_type.insert(std::make_pair("pptx", "application/vnd.openxmlformats-officedocument.presentationml.presentation"));
    _content_type.insert(std::make_pair("rar", "application/vnd.rar"));
    _content_type.insert(std::make_pair("rtf", "application/rtf"));
    _content_type.insert(std::make_pair("sh", "application/x-sh"));
    _content_type.insert(std::make_pair("svg", "image/svg+xml"));
    _content_type.insert(std::make_pair("swf", "application/x-shockwave-flash"));
    _content_type.insert(std::make_pair("tar", "application/x-tar"));
    _content_type.insert(std::make_pair("tif", "image/tiff"));
    _content_type.insert(std::make_pair("tiff", "image/tiff"));
    _content_type.insert(std::make_pair("ts", "video/mp2t"));
    _content_type.insert(std::make_pair("ttf", "font/ttf"));
    _content_type.insert(std::make_pair("txt", "text/plain"));
    _content_type.insert(std::make_pair("vsd", "application/vnd.visio"));
    _content_type.insert(std::make_pair("wav", "audio/wav"));
    _content_type.insert(std::make_pair("weba", "audio/webm"));
    _content_type.insert(std::make_pair("webm", "video/webm"));
    _content_type.insert(std::make_pair("webp", "image/webp"));
    _content_type.insert(std::make_pair("woff", "font/woff"));
    _content_type.insert(std::make_pair("woff2", "font/woff2"));
    _content_type.insert(std::make_pair("xhtml", "application/xhtml+xml"));
    _content_type.insert(std::make_pair("xls", "application/vnd.ms-excel"));
    _content_type.insert(std::make_pair("xlsx", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"));
    _content_type.insert(std::make_pair("xml", "text/xml"));
    _content_type.insert(std::make_pair("xul", "application/vnd.mozilla.xul+xml"));
    _content_type.insert(std::make_pair("zip", "application/zip"));
    _content_type.insert(std::make_pair("7z", "application/x-7z-compressed"));
}

void Server::initIndexPages() {
    _index.push_back("index.html");
}

std::string Server::makeAutoindexPage(std::string path) {
    std::cout << "MakeAutoIndexPage" << std::endl;
    std::string res = "<html>\n"
                      "<head><title>Index of /</title></head>\n"
                      "<body bgcolor=\"white\">\n"
                      "<h1>Index of /</h1><hr><pre><a href=\"../\">../</a>\n"
                      "<a href=\"456/\">456/</a>                                               19-Jan-2022 17:32                   -\n"
                      "<a href=\"789/\">789/</a>                                               19-Jan-2022 17:32                   -\n"
                      "<a href=\"hello_world.txt\">hello_world.txt</a>                                    19-Jan-2022 17:32                  14\n"
                      "</pre><hr></body>\n"
                      "</html>";

    std::string res2 = "<html>\n"
                       "<head><title>Index of "; /* + path + "</title></head>\n"
                       "<body bgcolor=\"white\">\n"
                       "<h1>Index of /</h1><hr><pre><a href=\"../\">../</a>\n"; */


    res2 += &path[1];
    res2 += "</title></head>\n"
            "<body bgcolor=\"white\">\n"
            "<h1>Index of ";
    res2 += &path[1];
    res2 += "</h1><hr><pre><a href=\"../\">../</a>\n";
    std::list<struct dirent>  content;
    Utils::getDirContent(path.c_str(), content);
    for (std::list<struct dirent>::iterator it = content.begin(); it != content.end(); ++it) {
        if (!strcmp(it->d_name, ".") || !strcmp(it->d_name, ".."))
            continue;
        res2 += makeAutoindexLine(it, path);
    }
    res2 += "</pre><hr></body>\n"
            "</html>";

    return res2;
}

std::string Server::makeAutoindexLine(std::list<struct dirent>::iterator file, const std::string &path) {
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

Client *Server::getClient(int client_sock) {
    for (std::list<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if ((*it)->getSocket() == client_sock)
            return (*it);
    }

    return NULL;
}

const int &Server::getServSocket() const { return _socket; }

int Server::setupServer() {

    int                 serv_sock;
    struct sockaddr_in  serv_addr;

    if ((serv_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket error!");
        exit(errno);
    }
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(_port);

    int opt = 1;
    setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Bind error!");
        exit(errno);
    }
    if (listen(serv_sock, _backlog) < 0) {
        perror("Listen error!");
        exit(errno);
    }
    _max_fd = serv_sock;

    return serv_sock;
}

bool Server::addNewClient(int client_sock) {
    for (std::list<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if ((*it)->getSocket() == client_sock)
            return false;
    }
    _clients.push_back(new Client(client_sock));

    return true;
}

void Server::removeClient(int client_sock, fd_set &curr_sock) {
    FD_CLR(client_sock, &curr_sock);
    close(client_sock);
    for (std::list<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if ((*it)->getSocket() == client_sock) {
            _clients.erase(it);
            std::cout << "Client " << (*it)->getSocket() << " has been removed. Closing connection..."  << std::endl;
            delete *it;
            break;
        }
    }
}

bool Server::isKeepAlive(const std::vector<std::string>& data_lines) {
    for (int i = 0; i < data_lines.size(); ++i) {
        if (strstr(data_lines[i].c_str(), "Connection: keep-alive"))
            return true;
    }
    return false;
}

int Server::acceptNewConnection() {

    size_t              addr_size = sizeof(struct sockaddr_in);
    int                 client_sock;
    struct sockaddr_in  client_addr;

    if ((client_sock = accept(_socket, (struct sockaddr*)&client_addr, (socklen_t*)&addr_size)) < 0) {
        perror("Accept error!");
        exit(errno);
    }
    fcntl(client_sock, F_SETFL, O_NONBLOCK);
    if (!addNewClient(client_sock)) {
        std::cout << "Client " << client_sock << " has not been added to list. Closing connection..." << std::endl;
        close(client_sock);
    }
    if (client_sock > _max_fd)
        _max_fd = client_sock;
    return client_sock;
}

void Server::makeResponseHeader(const std::string& status, const std::string& content_type, const std::string& file_time) {
    _response_header.clear();
    _response_header.append("HTTP/1.1 " + status + "\n");
    _response_header.append("Date: " + Utils::getTime() + "\n");
    _response_header.append("Server: " + _name + "\n");
    if (status == "200 OK" || status == "201 CREATED")
        _response_header.append("Last-Modified: " + file_time + "\n");
    _response_header.append("Accept-Ranges: bytes\n");
    _response_header.append("Content-Length: " + std::to_string(_response_data_size) + "\n");
    if (!content_type.empty())
        _response_header.append("Content-Type: " + content_type + "\n\n");
}

std::string Server::defineContentType(const std::string& file_path) const {
    std::string     extension = Utils::getExtension(file_path);

    try {
        std::cout << "file extension: " << extension << std::endl;
        const std::string& result = _content_type.at(extension);
        return result;
    }
    catch (std::exception& ex) {
        std::cout << ex.what() << std::endl;
        std::cout << "Content-Type has been set to text\\html" << std::endl;
        return "text/html";
    }
}

void Server::makeGetResponse(Client *client) {
    int                         path_exist = false;
    int                         path_access = false;
    std::string                 request_path = client->getRequestPath();
    const char*                 file_path;
    std::string                 file_time;
    std::string                 content_type;
    std::string                 response_status;
    std::pair<char*, size_t>    file_data;

    std::cout << "Path requested: " << request_path << std::endl;

    boolalpha(std::cout);
    if (request_path == "/") {
        std::cout << "Index requested" << std::endl;

        for (std::list<std::string>::iterator it = _index.begin(); it != _index.end(); ++it) {
            if ((path_exist = Utils::isPathExist(*it))) {
                file_path = (*it).c_str();
                break;
            }
        }
        if (!path_exist) {
            std::cout << "Path does not exist" << std::endl;
            if (_autoindex) {
                std::cout << "autoindex on, sending root page" << std::endl;

                response_status = "200 OK";
                content_type = "text/html";
                file_time = Utils::getTime();
                std::string autoindex_page = makeAutoindexPage("./");
                size_t autoindex_size = autoindex_page.size();
                _response_data = new char[autoindex_size];
                _response_data_size = autoindex_size;
                std::memcpy(_response_data, autoindex_page.c_str(), autoindex_size);
                makeResponseHeader(response_status, content_type, file_time);
                _response_full_size = _response_header.size() + _response_data_size;

                return;
            }
            else {
                std::cout << "autoindex off, sending error page" << std::endl;

                file_path = "error_pages/403.html";
                response_status = "403 FORBIDDEN";
            }
        }
    } else if (request_path[request_path.length() - 1] == '/') {
        std::cout << "Directory requested" << std::endl;

        file_path = &request_path[1];
        if (_autoindex) {
            std::cout << "Autoindex on" << std::endl;
            path_exist = Utils::isPathExist(file_path);
            if (path_exist) {
                std::cout << "Path exists" << std::endl;
                path_access = (access(file_path, R_OK) == 0);
//                path_access = Utils::isPathAccessed(file_path);
                if (path_access) {
                    std::cout << "Path access granted" << std::endl;

                    response_status = "200 OK";
                    content_type = "text/html";
                    file_time = Utils::getTime();
                    std::string autoindex_page = makeAutoindexPage("." + request_path);
//                    std::cout << "autoindex page: " << autoindex_page << std::endl;
                    size_t autoindex_size = autoindex_page.size();
//                    std::cout << "autoindex size: " << autoindex_size << std::endl;
                    _response_data = new char[autoindex_size];
                    _response_data_size = autoindex_size;
                    std::memcpy(_response_data, autoindex_page.c_str(), autoindex_size);
                    makeResponseHeader(response_status, content_type, file_time);
                    _response_full_size = _response_header.size() + _response_data_size;

                    return;
                } else {
                    std::cout << "Path access denied" << std::endl;
                    file_path = "error_pages/403.html";
                    response_status = "403 FORBIDDEN";
                }
            } else {
                std::cout << "Path does not exist" << std::endl;
                file_path = "error_pages/404.html";
                response_status = "404 NOT FOUND";
            }
        } else {
            std::cout << "Autoindex off, GET dir denied, sending error page" << std::endl;
            file_path = "error_pages/403.html";
            response_status = "403 FORBIDDEN";
        }
    } else {
        std::cout << "File requested" << std::endl;

        file_path = &request_path[1];
        path_exist = Utils::isPathExist(file_path);
        if (path_exist) {
            std::cout << "Path exists" << std::endl;
            path_access = (access(file_path, R_OK) == 0);
            if (path_access) {
                std::cout << "Path access granted" << std::endl;
                response_status = "200 OK";
                file_time = Utils::getFileLastModTime(file_path);
            } else {
                std::cout << "Path access denied" << std::endl;
                response_status = "403 FORBIDDEN";
            }
        } else {
            std::cout << "File does not exist" << std::endl;
            file_path = "error_pages/404.html";
            response_status = "404 NOT FOUND";
        }
    }
    std::cout << "result path: " << file_path << std::endl;
    file_data = Utils::readFile(file_path);
    if (file_data.first) {
        _response_data = file_data.first;
        _response_data_size = file_data.second;
    } else {
        std::cout << "Can't read file" << std::endl;
        response_status = "204 NO CONTENT";
    }

    content_type = defineContentType(file_path);
    makeResponseHeader(response_status, content_type, file_time);
    _response_full_size = _response_header.size() + _response_data_size;


    /* autoindex on
     *
     * index2.html? -> set in conf? [if not: path = root dir] -> exist? [if not: path = root dir] -> access? -> send file / error page / make autoindex page root dir
     * file? -> exist? -> access? -> send file / error page
     * directory? -> exist? -> access? -> make autoindex page this dir / error page
     */

    /* autoindex off
     *
     * index2.html? -> set in conf? [if not: path = root dir] -> exist? [if not: path = root dir] -> access? [no access to dir in ai = off] -> send file / error page
     * file? -> exist? -> access? -> send file / error page
     * directory? -> [no access to dir in ai = off]
     */
}

void Server::makePostResponse(Client *client) {
    struct stat fi;
    std::string data_path = "data_from_client_id_" + std::to_string(client->getId()) + "." + client->getPostContentType();
    std::string file_time, response_status;
    bool        read_ok;

    bzero(&fi, sizeof(fi));
    std::cout << "fact data path: " << data_path << std::endl;
    stat(data_path.c_str(), &fi);
    std::cout << "Fact file size: " << fi.st_size << std::endl;
    std::cout << "POST content length: " << client->getPostContentLen() << std::endl;
    if (fi.st_size == client->getPostContentLen()) {
        response_status = "201 CREATED";
        file_time = Utils::getFileLastModTime(data_path);
    } else {
        response_status = "400 BAD REQUEST";
    }
    _response_data_size = 0;
    makeResponseHeader(response_status, client->getPostContentType(), file_time);
    _response_full_size = _response_header.size();
}

void Server::makeDeleteResponse(Client *client) {

    bool                        path_exist, remove_ok; //, read_ok;
    std::string                 path = "." + client->getRequestPath();
    std::string                 response_status;
    std::string                 html_path;
    std::string                 file_time;
    std::string                 content_type;
    std::pair<char*, size_t>    file_data;

    std::cout << "Path DELETE requested: " << path << std::endl;
    path_exist = !access(path.c_str(), F_OK);
    if (path_exist) {
        std::cout << "Path exists" << std::endl;
        remove_ok = !remove(path.c_str());
        if (remove_ok) {
            std::cout << "Remove done" << std::endl;
            response_status = "200 OK";
            html_path = "./deleted.html";
        } else {
            std::cout << "Remove ERROR" << std::endl;
            response_status = "500 INTERNAL SERVER ERROR";
            html_path = "./error_pages/500.html";
        }
    } else {
        std::cout << "Path does not exist" << std::endl;
        html_path = "./error_pages/404.html";
        response_status = "404 NOT FOUND";
    }
    file_time = Utils::getFileLastModTime(html_path);
    file_data = Utils::readFile(html_path);
    if (file_data.first) {
        _response_data = file_data.first;
        _response_data_size = file_data.second;
    } else {
        std::cout << "Can't read file" << std::endl;
        response_status = "204 NO CONTENT";
    }
    content_type = defineContentType(html_path);
    makeResponseHeader(response_status, content_type, file_time);
    _response_full_size = _response_header.size() + _response_data_size;
}

bool Server::handlePostData(Client *client, const char *begin, const char *end) {

    long            size = end - begin;
    std::string     file_name = "." + client->getRequestPath() + "/data_from_client_id_"
            + std::to_string(client->getId()) + "." + client->getPostContentType();
    std::ofstream   ofs;

    std::cout << "Data path: " << file_name << std::endl;
    std::cout << "Data size: " << size << std::endl;
    ofs.open(file_name, std::fstream::binary);
    if (ofs.is_open()) {
        ofs.write(begin, size);
        if (ofs.fail()) {
            std::cout << "Error: Can't write data from Client " << client->getSocket() << std::endl;
            return false;
        }
    } else {
        std::cout << "Error: Can't open file to save data from Client " << client->getSocket() << std::endl;
        return false;
    }
    return true;
}

bool Server::handlePostHeader(Client *client) {

    std::vector<std::string>    header_line = Utils::split(client->getRequestHeader(), '\n');
    const char                  *cont_type;
    std::string                 file_type;
    long long int               length;
    bool                        len_ok, type_ok = false;
    Utils::t_find_key           type_search;

    { // system debug
        for (std::vector<std::string>::iterator it = header_line.begin(); it != header_line.end(); ++it) {
            std::cout << *it << std::endl;
        }
        std::cout << "$" << std::endl;
    }

    if (header_line.size() < 3)
        return false;
    for (int i = 0; i < header_line.size(); ++i) {
        if (!strncmp(header_line[i].c_str(), "Content-Length", strlen("Content-Length"))) {
            length = std::strtol(strchr(header_line[i].c_str(), ' '), 0, 0);
            if (!length || length > _max_body_size) {
                return false;
            }
            client->setPostContentLen(length);
            type_ok = true;
        }
        if (!strncmp(header_line[i].c_str(), "Content-Type", strlen("Content-Type"))) {
            cont_type = strchr(header_line[i].c_str(), ' ') + 1;
            if (!cont_type)
                return false;
            std::cout << "cont_type: " << cont_type << std::endl;
            type_search = Utils::findKey(_content_type.cbegin(), _content_type.cend(), cont_type);
            if (type_search == _content_type.cend()) {
                std::cout << "Unknown file type" << std::endl;
                client->setPostContentType("");
            } else
                client->setPostContentType(type_search->first);
            len_ok = true;
        }
    }
    std::cout << "POST header is OK" << std::endl;
    return len_ok && type_ok;
}

std::string Server::extractRequestPath(const char *buffer) {
    std::string     result_path;
    const char      *first_space_pos;
    const char      *second_space_pos;

    if (buffer) {
        first_space_pos = strchr(buffer, ' ');
        if (!first_space_pos || !(first_space_pos + 1))
            return "";
        second_space_pos = strchr(first_space_pos + 1, ' ');
        if (!second_space_pos)
            return "";
        result_path.assign(first_space_pos + 1, second_space_pos);
    }
    return result_path;
}

int Server::countEmptyHeaderLines(const char *buffer, unsigned int request_type) {

    int empty_ln = 0;

    if (request_type != POST_RQST) {
        if (buffer[0] == '\r') {
            for (; empty_ln < BUFFSIZE && buffer[empty_ln] == '\r' && buffer[empty_ln + 1] == '\n'; empty_ln += 2) {}
        }
    }

    return empty_ln;
}

unsigned Server::defineRequestType(const char *buffer) {
    if (!strncmp(buffer, "GET", strlen("GET")))
        return GET_RQST;
    else if (!strncmp(buffer, "POST", strlen("POST")))
        return POST_RQST;
    else if (!strncmp(buffer, "DELETE", strlen("DELETE")))
        return DELETE_RQST;
    else
        return UNKNOWN_RQST;
}

header_params Server::defineHeaderParams(const char *buffer) {

}

void Server::handleRequest(int client_sock, fd_set &curr_sock) {

    char            buffer[BUFFSIZE + 1];
    size_t          bytes_read;
    std::string     msg = "";
    std::string     response;
    std::string     request_path;
    Client          *client;
    size_t          header_size;
    char*           header_end;
    bool            get, post, del;
    int             empty_ln;
    unsigned        request_type;

    bytes_read = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
    buffer[bytes_read] = '\0';
    if (bytes_read < 0) {
        std::cout << "Read socket: Error has been occurred" << std::endl;
        removeClient(client_sock, curr_sock);
        return;
    }
    if (bytes_read == 0) {
        std::cout << "Client " << client_sock << " has closed the connection" << std::endl;
        removeClient(client_sock, curr_sock);
        return;
    }
    if (!(client = getClient(client_sock))) {
        std::cout << "Error: getClient(): can't find client" << std::endl;
        removeClient(client_sock, curr_sock);
        return;
    }

    empty_ln = countEmptyHeaderLines(buffer, client->getRequestType()); // BUT IF DATA RECEIVED ?? AND IT CONTAINS /r/n
    request_type = defineRequestType(buffer + empty_ln);

    if (request_type) {

        header_end = strstr(buffer + empty_ln, "\r\n\r\n");
        if (header_end) {
            std::cout << "END DETECTED" << std::endl;
            header_end += strlen("\r\n\r\n");
            header_size = header_end - buffer + empty_ln;
        }
        else {
            std::cout << "NO END" << std::endl;
            header_end = buffer + bytes_read;
            header_size = 0;
        }

        client->setRequestHeader(buffer + empty_ln, header_end);
        request_path = extractRequestPath(buffer);
        client->setRequestPath(request_path);

        if (request_type == GET_RQST) {
            std::cout << "GET REQUEST" << std::endl;
            client->setRequestType(request_type);
            if (header_size != 0 && header_end < buffer + bytes_read)
                client->writeData(header_end, buffer + bytes_read);
        }
        else if (request_type == POST_RQST) {
            std::cout << "POST REQUEST" << std::endl;
            client->setRequestType(request_type);
            if (!handlePostHeader(client)) {
                std::cout << "POST request header error" << std::endl;
                removeClient(client_sock, curr_sock);
                return;
            }

            { // system debug
                std::cout << "header size: " << header_size << std::endl;
                std::cout << "header end address: " << static_cast<void *>(header_end) << std::endl;
                std::cout << "buffer + bytes_read address: " << static_cast<void *>(buffer + bytes_read) << std::endl;
            }

            if (header_size != 0 && header_end < buffer + bytes_read) {
                std::cout << "Prepare to handle POST data..." << std::endl;
                if (!handlePostData(client, header_end, buffer + bytes_read)) {
                    removeClient(client_sock, curr_sock);
                    return;
                } else {
                    std::cout << "Client " << client->getSocket() << " data has been successfully written" << std::endl;
                }
            }
        }
        else if (request_type == DELETE_RQST) {
            std::cout << "DELETE REQUEST" << std::endl;
            client->setRequestType(request_type);
        }
        else {
            std::cout << "Error: Request type" << std::endl;
            client->setRequestType(0);
        }

    } else if (client->getRequestType() == POST_RQST) {
        std::cout << "POST REQUEST DATA" << std::endl;
        if (!handlePostData(client, buffer, buffer + bytes_read)) {
            removeClient(client_sock, curr_sock);
            return;
        }

    } else {
        std::cout << "BAD REQUEST" << std::endl;
        removeClient(client_sock, curr_sock);
        return;
    }

    client->setProcessed(false);
    std::cout << "Client " << client->getSocket() << " bytes read: " << bytes_read << std::endl;
}

void Server::sendResponse(int client_sock, fd_set& curr_sock, fd_set& write_sock) {
    Client*                     client;
    std::vector<uint8_t>        response;
    std::vector<std::string>    header;

    if (!(client = getClient(client_sock))) {
        std::cout << "Error: getClient(): can't find client" << std::endl;
        removeClient(client_sock, curr_sock);
        return;
    }

    const std::vector<uint8_t>&        data = client->getData();

//    if (data.size() >= 2 && *data.rbegin() == '\n' && (*data.rbegin() + 1) == '\n')
//        return;
    if (data.size() >= 4 && *data.rbegin() == '\n' && (*data.rbegin() + 1) == '\r'
    && (*data.rbegin() + 2) == '\n' && (*data.rbegin() + 3) == '\n') {
        // 104 CONTINUE response
        return;
    }


//    if (data.find("\r\n\r\n", data.length() - END_LEN) == std::string::npos
//    && data.find("\n\n", data.length() - 2) == std::string::npos)
//        return;


    header = Utils::split(client->getRequestHeader(), '\n');
    if (client->getRequestType() == GET_RQST) {
        std::cout << "Prepare GET response" << std::endl;
        makeGetResponse(client);
    } else if (client->getRequestType() == POST_RQST) {
        std::cout << "Prepare POST response" << std::endl;
        makePostResponse(client);
    } else if (client->getRequestType() == DELETE_RQST) {
        std::cout << "Prepare DELETE response" << std::endl;
        makeDeleteResponse(client);
    } else {

        // ???
        makeResponseHeader("405 NOT ALLOWED", "text/html", 0);
        std::cout << "Error: Response type" << std::endl;
    }

    std::cout << "Data to send size: " << _response_full_size << std::endl;
    try {
        uint8_t* data_to_send = new uint8_t[_response_full_size];

        std::memcpy(data_to_send, _response_header.c_str(), _response_header.size());
        if (client->getRequestType() == GET_RQST || client->getRequestType() == DELETE_RQST) {
            std::memcpy(data_to_send + _response_header.size(), _response_data, _response_data_size);
        }
        _log_data = data_to_send;
        send(client_sock, data_to_send, _response_full_size, 0);
    } catch (std::exception& ex) {
        std::cout << "Shutting down server..." << std::endl;
        sleep(DEBUG_DELAY_SEC);
        exit(1);
    }


    { // system debug
        std::string msg;

        std::cout << "---------------------------------------------------------------------------" << std::endl;
        std::cout << "Sending response for Client " << client->getSocket() << std::endl << std::endl;
        std::cout << "REQUEST from Client " << client->getSocket() << std::endl;
        msg.append(client->getRequestHeader());
        if (!client->getData().empty()) {
            msg.append((char*)(&client->getData()[0]));
        }

        std::cout << msg << std::endl;
        std::cout << "---------------------------------------------------------------------------" << std::endl;
        std::cout << "RESPONSE HEADER | size = " << _response_header.size() << std::endl;
        std::cout << _response_header;
        std::cout << "$" << std::endl;
        std::cout << "---------------------------------------------------------------------------" << std::endl;
        if (client->getRequestType() == GET_RQST) {
            std::cout << "RESPONSE DATA | size = " << _response_data_size << std::endl;
            if (_response_data)
                std::cout << std::string(_response_data) << std::endl;
            std::cout << "---------------------------------------------------------------------------" << std::endl;
            std::cout << "RESPONSE to Client " << client->getSocket() << " | size = " << _response_full_size << std::endl;
            std::cout << std::string((char*)_log_data) << std::endl;
            std::cout << "---------------------------------------------------------------------------" << std::endl;
        }
    }

    client->clearData();
    client->setProcessed(true);
    FD_CLR(client_sock, &write_sock);

    if (!isKeepAlive(header)) {
        std::cout << "No keep-alive" << std::endl;
        std::cout << "Client " << client->getSocket() << " has been successfully handled" << std::endl;
        removeClient(client_sock, curr_sock);
        return;
    }
}

void Server::addClientsToWriteSet(fd_set &write_sock) {
    for (std::list<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if (!(*it)->isProcessed()) // && !(*it)->getData().empty())  mb it's excess ??
            FD_SET((*it)->getSocket(), &write_sock);
    }
}

void* Server::checkClientsTimeout(void *data) {
    t_timeout_data     *timeout_data = static_cast<t_timeout_data*>(data);
    struct timeval      tv;

    bzero(&tv, sizeof(tv));
    for (;true;) {
        usleep(CHECKER_TIMEOUT_DELAY_USEC);
        gettimeofday(&tv, NULL);
        for (std::list<Client*>::iterator it = timeout_data->clients->begin(); it != timeout_data->clients->end(); ++it) {
            if (tv.tv_sec - (*it)->getLastActionTime().tv_sec >= CLIENT_TIMEOUT_DELAY_SEC) {
                pthread_mutex_lock(timeout_data->mutex);
                int client_sock = (*it)->getSocket();
                std::cout << "Client " << client_sock << " timeout" << std::endl;
                std::cout << "Closing " << client_sock << " connection..." << std::endl;
                delete *it;
                timeout_data->clients->erase(it);
                close(client_sock);
                FD_CLR(client_sock, timeout_data->curr_sock);
                pthread_mutex_unlock(timeout_data->mutex);
            }
        }
    }
}

void Server::initTimeoutThread(pthread_t **timeout_thread, t_timeout_data *data, fd_set *curr_sock) {
    *timeout_thread = new pthread_t;

    data->curr_sock = curr_sock;
    data->mutex = new t_mutex;
    data->clients = &_clients;
    pthread_create(*timeout_thread, 0, &Server::checkClientsTimeout, data);
    pthread_mutex_init(data->mutex, NULL);
}

int Server::countReadySockets(fd_set *read_sock, fd_set *write_sock) {
    int num_ready_sock = 0;

    for (int i = 0; i <= _max_fd; ++i) {
        if (FD_ISSET(i, read_sock) || FD_ISSET(i, write_sock))
            num_ready_sock++;
    }
    return num_ready_sock;
}

bool Server::start() {

    int             client_socket, prev_sock = 0, num_ready_sock = 0;
    fd_set          curr_sock, read_sock, write_sock;
    pthread_t       *timeout_thread;
    t_timeout_data  timeout_data;

    _socket = setupServer();
    FD_ZERO(&curr_sock);
    FD_SET(_socket, &curr_sock);
    write_sock = read_sock = curr_sock;
    initTimeoutThread(&timeout_thread, &timeout_data, &curr_sock);
    for (;;) {

        { // system debug
            sleep(DEBUG_DELAY_SEC);
//            std::cout << "" << std::endl;
            std::cout << "***************************************************************************" << std::endl;
//            std::cout << "i: " << i << std::endl;
            std::cout << "max fd: " << _max_fd << std::endl;
            std::cout << "prev sock: " << prev_sock << std::endl;
            std::cout << "num ready sock: " << num_ready_sock << std::endl;
        }

        pthread_mutex_lock(timeout_data.mutex);
        read_sock = curr_sock;
        FD_ZERO(&write_sock);
        addClientsToWriteSet(write_sock);
        if (select(_max_fd + 1, &read_sock, &write_sock, 0, 0) < 0) {
            perror("Select error");
            exit(errno);
        }
        num_ready_sock = countReadySockets(&read_sock, &write_sock);
        std::cout << "num ready sock after: " << num_ready_sock << std::endl;
        std::cout << "" << std::endl;
        for (int fd = 0; fd <= _max_fd; ++fd) {
            if (prev_sock == _max_fd)
                prev_sock = 0;
            if (num_ready_sock > 1 && fd == prev_sock)
                continue;
            if (FD_ISSET(fd, &read_sock)) {
                prev_sock = fd;
                if (fd == _socket) {
                    sleep(DEBUG_DELAY_SEC);
                    std::cout << "acceptNewConnection..." << std::endl << std::endl;
                    client_socket = acceptNewConnection();
                    sleep(DEBUG_DELAY_SEC);
                    std::cout << "Socket " << client_socket << " has been created" << std::endl;
                    FD_SET(client_socket, &curr_sock);
                    break;
                } else {
                    sleep(DEBUG_DELAY_SEC);
                    std::cout << "handleRequest from " << fd << "..." << std::endl << std::endl;
                    handleRequest(fd, curr_sock);
                    break;
                }
            } else if (FD_ISSET(fd, &write_sock)) {
                prev_sock = fd;
                sleep(DEBUG_DELAY_SEC);
                std::cout << "sendResponse to " << fd << "..." << std::endl << std::endl;
                sendResponse(fd, curr_sock, write_sock);
                break;
            }
        }
        pthread_mutex_unlock(timeout_data.mutex);
    }
    pthread_join(*timeout_thread, 0);
}

