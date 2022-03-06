/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cshells <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/02 15:26:26 by cshells           #+#    #+#             */
/*   Updated: 2022/02/02 15:26:28 by cshells          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"

int WebServer::id_generator = -1;

WebServer::WebServer(EventSelector *event_selector, int fd, const Config &config)
    : FdHandler(fd, true), _event_selector(event_selector), _config(config), _display_name("webserv") {

    _id = ++id_generator;
    initServerTraits();
    initLocations();
    initContentTypes();
    std::cout << "WebServer: Joining to EventSelector..." << std::endl;
    _event_selector->add(this);
}

WebServer::~WebServer() {

    for (std::list<WebSession*>::iterator it = _session_list.begin(); it != _session_list.end(); ++it) {
        _event_selector->remove((*it));
    }
    _event_selector->remove(this);
}

WebServer *WebServer::start(EventSelector *event_selector, int port, const Config &config) {

    int                 listen_socket, option, result;
    struct sockaddr_in  addr;

    std::cout << "WebServer: Creating socket..." << std::endl;
    listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_socket == -1) {
        return 0;
    }

    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    std::cout << "WebServer: Setting socket options..." << std::endl;
    option = 1;
    setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    std::cout << "WebServer: Binding socket..." << std::endl;
    result = bind(listen_socket, (struct sockaddr*)&addr, sizeof(addr));
    if (result == -1)
        return 0;

    result = listen(listen_socket, backlog);
    if (result == -1)
        return 0;

    return new WebServer(event_selector, listen_socket, config);
}

void WebServer::Handle(bool read, bool write) {

    if (!read)
        return;
    int                 client_socket;
    struct sockaddr_in  addr;
    socklen_t           len = sizeof(addr);

    std::cout << "WebServer: Accepting new connection..." << std::endl;
    client_socket = accept(this->getFd(), (struct sockaddr*)&addr, &len);
    if (client_socket == -1)
        return;

    fcntl(client_socket, F_SETFL, O_NONBLOCK);

    sleep(DEBUG_DELAY_SEC);
    WebSession  *session = new WebSession(this, client_socket);
    _session_list.push_front(session);
    _event_selector->add(session);
    std::cout << "WebServer: Connection " << client_socket << " has been created" << std::endl;
}

void WebServer::removeSession(WebSession *session) {

    _event_selector->remove(session);
    for (std::list<WebSession*>::iterator it = _session_list.begin(); it != _session_list.end(); ++it) {
        if ((*it) == session) {
            std::cout << "WebServer: WebSession " << session->getFd() << " has been removed. Closing connection..."  << std::endl;
            _session_list.erase(it);
            delete *it;
            return;
        }
    }
}

void WebServer::initServerTraits() {
    std::cout << "WebServer: Initializing server's traits..." << std::endl;

    initSingleTrait(_server_name, "server_name");
    initSingleTrait(_listen_addr, "listen");
    initSingleTrait(_traits._root, "root");
    initSingleTrait(_traits._autoindex, "autoindex", Utils::strToBool);
    initSingleTrait(_traits._client_body_size, "client_body_size", Utils::strToLongLong);
    initMultipleTrait(_traits._methods_list, "methods");
    initMultipleTrait(_traits._index_list, "index");

//    for (list<string>::const_iterator it = _traits._index_list.cbegin(); it != _traits._index_list.end(); ++it) {
//        std::cout << *it << std::endl;
//    }
//
//    for (list<string>::const_iterator it = _traits._methods_list.cbegin(); it != _traits._methods_list.end(); ++it) {
//        std::cout << *it << std::endl;
//    }
}

void WebServer::initLocations() {
    std::cout << "WebServer: Initializing locations' traits..." << std::endl;

    const set<string> &locations = _config.getServerLocations(_id);

    for (set<string>::const_iterator location_iter = locations.cbegin(); location_iter != locations.cend(); ++location_iter) {
        LocationTraits  location(_traits, *location_iter);

        string  trait_name = "location " + *location_iter;
        initSingleTrait(location._root, trait_name + ".root");
        initSingleTrait(location._client_body_size, trait_name + ".client_body_size", Utils::strToLongLong);
        initSingleTrait(location._autoindex, trait_name + ".autoindex", Utils::strToBool);
        initMultipleTrait(location._index_list, trait_name + ".index");
        initMultipleTrait(location._methods_list, trait_name + ".methods");
        _location_list.push_front(location);

        std::cout << "location: " << *location_iter << std::endl;
        std::cout << "root: " << location.getRoot() << std::endl;
        std::cout << "client_body_size: " << location.getClientBodySize() << std::endl;
        std::cout << "autoindex: " << location.isAutoindex() << std::endl;

        const list<string> &methods_list = location.getMethodsList();

        for (list<string>::const_iterator it = methods_list.cbegin(); it != methods_list.cend(); ++it) {
            std::cout << *it;
            std::cout << " ";
        }
        std::cout << "" << std::endl;
        std::cout << "---------------------------------------------------" << std::endl;
    }
}

void WebServer::initSingleTrait(string &trait, const string &trait_name) {
    if (_config.hasServerTrait(_id, trait_name)) {
        trait = _config[to_string(_id) + ".server." + trait_name];
    }
}

template <class T>
void WebServer::initSingleTrait(T &trait, const string &trait_name, T (*strToT)(const string &str)) {

    if (_config.hasServerTrait(_id, trait_name)) {
        try {
            trait = strToT(_config[to_string(_id) + ".server." + trait_name]);
        } catch (std::exception&) {
            trait = T();
        }
    }
}

void WebServer::initMultipleTrait(list<string> &trait_list, const string &trait_name) {
    if (!_config.hasServerTrait(_id, trait_name))
    {
        std::cout << "trait_name: " << trait_name << "$ return" << std::endl;
        return;
    }
    std::cout << "here" << std::endl;
    trait_list.clear();
    vector<string>    traits = Utils::split(_config[to_string(_id) + ".server." + trait_name], ',');

    for (vector<string>::iterator it = traits.begin(); it != traits.end(); ++it) {
        trait_list.push_front(*it);
    }
}

void WebServer::initContentTypes() {
    _content_types.insert(std::make_pair("aac", "audio/aac"));
    _content_types.insert(std::make_pair("abw", "application/x-abiword"));
    _content_types.insert(std::make_pair("arc", "application/x-freearc"));
    _content_types.insert(std::make_pair("avi", "video/x-msvideo"));
    _content_types.insert(std::make_pair("azw", "application/vnd.amazon.ebook"));
    _content_types.insert(std::make_pair("bin", "application/octet-stream"));
    _content_types.insert(std::make_pair("bmp", "image/bmp"));
    _content_types.insert(std::make_pair("bz", "application/x-bzip"));
    _content_types.insert(std::make_pair("bz2", "application/x-bzip2"));
    _content_types.insert(std::make_pair("csh", "application/x-csh"));
    _content_types.insert(std::make_pair("css", "text/css"));
    _content_types.insert(std::make_pair("csv", "text/csv"));
    _content_types.insert(std::make_pair("doc", "application/msword"));
    _content_types.insert(std::make_pair("docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document"));
    _content_types.insert(std::make_pair("eot", "application/vnd.ms-fontobject"));
    _content_types.insert(std::make_pair("epub", "application/epub+zip"));
    _content_types.insert(std::make_pair("gz", "application/gzip"));
    _content_types.insert(std::make_pair("gif", "image/gif"));
    _content_types.insert(std::make_pair("htm", "text/html"));
    _content_types.insert(std::make_pair("html", "text/html"));
    _content_types.insert(std::make_pair("ico", "image/vnd.microsoft.icon"));
    _content_types.insert(std::make_pair("ics", "text/calendar"));
    _content_types.insert(std::make_pair("jar", "application/java-archive"));
    _content_types.insert(std::make_pair("jpeg", "image/jpeg"));
    _content_types.insert(std::make_pair("jpg", "image/jpeg"));
    _content_types.insert(std::make_pair("js", "text/javascript"));
    _content_types.insert(std::make_pair("json", "application/json"));
    _content_types.insert(std::make_pair("jsonld", "application/ld+json"));
    _content_types.insert(std::make_pair("mid", "audio/midi audio/x-midi"));
    _content_types.insert(std::make_pair("midi", "audio/midi audio/x-midi"));
    _content_types.insert(std::make_pair("mjs", "text/javascript"));
    _content_types.insert(std::make_pair("mp3", "audio/mpeg"));
    _content_types.insert(std::make_pair("mpeg", "video/mpeg"));
    _content_types.insert(std::make_pair("mpkg", "application/vnd.apple.installer+xml"));
    _content_types.insert(std::make_pair("odp", "application/vnd.oasis.opendocument.presentation"));
    _content_types.insert(std::make_pair("ods", "application/vnd.oasis.opendocument.spreadsheet"));
    _content_types.insert(std::make_pair("odt", "application/vnd.oasis.opendocument.text"));
    _content_types.insert(std::make_pair("oga", "audio/ogg"));
    _content_types.insert(std::make_pair("ogv", "video/ogg"));
    _content_types.insert(std::make_pair("ogx", "application/ogg"));
    _content_types.insert(std::make_pair("opus", "audio/opus"));
    _content_types.insert(std::make_pair("otf", "font/otf"));
    _content_types.insert(std::make_pair("png", "image/png"));
    _content_types.insert(std::make_pair("pdf", "application/pdf"));
    _content_types.insert(std::make_pair("php", "application/php"));
    _content_types.insert(std::make_pair("ppt", "application/vnd.ms-powerpoint"));
    _content_types.insert(std::make_pair("pptx", "application/vnd.openxmlformats-officedocument.presentationml.presentation"));
    _content_types.insert(std::make_pair("rar", "application/vnd.rar"));
    _content_types.insert(std::make_pair("rtf", "application/rtf"));
    _content_types.insert(std::make_pair("sh", "application/x-sh"));
    _content_types.insert(std::make_pair("svg", "image/svg+xml"));
    _content_types.insert(std::make_pair("swf", "application/x-shockwave-flash"));
    _content_types.insert(std::make_pair("tar", "application/x-tar"));
    _content_types.insert(std::make_pair("tif", "image/tiff"));
    _content_types.insert(std::make_pair("tiff", "image/tiff"));
    _content_types.insert(std::make_pair("ts", "video/mp2t"));
    _content_types.insert(std::make_pair("ttf", "font/ttf"));
    _content_types.insert(std::make_pair("txt", "text/plain"));
    _content_types.insert(std::make_pair("vsd", "application/vnd.visio"));
    _content_types.insert(std::make_pair("wav", "audio/wav"));
    _content_types.insert(std::make_pair("weba", "audio/webm"));
    _content_types.insert(std::make_pair("webm", "video/webm"));
    _content_types.insert(std::make_pair("webp", "image/webp"));
    _content_types.insert(std::make_pair("woff", "font/woff"));
    _content_types.insert(std::make_pair("woff2", "font/woff2"));
    _content_types.insert(std::make_pair("xhtml", "application/xhtml+xml"));
    _content_types.insert(std::make_pair("xls", "application/vnd.ms-excel"));
    _content_types.insert(std::make_pair("xlsx", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"));
    _content_types.insert(std::make_pair("xml", "text/xml"));
    _content_types.insert(std::make_pair("xul", "application/vnd.mozilla.xul+xml"));
    _content_types.insert(std::make_pair("zip", "application/zip"));
    _content_types.insert(std::make_pair("7z", "application/x-7z-compressed"));
}

//const list<Location *> WebServer::getLocationList() const { return _location_list; }

const map<string, string> WebServer::getContentTypes() const { return _content_types; }

const list<string> &WebServer::getIndexList() const { return _traits._index_list; }