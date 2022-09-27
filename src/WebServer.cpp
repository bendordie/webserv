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

WebServer::WebServer(EventSelector *event_selector, int port, int fd, const Config &config, int indexInConfig)
    : FdHandler(fd, true), _port(port), _indexInConfig(indexInConfig),
      _config(config), _eventSelector(event_selector), _timeoutSec(global::SESSION_TIMEOUT_SEC), _timeoutMutex(event_selector->getTimeoutMutex()) {

    _timeoutHandler = thread(&WebServer::checkSessionTimeout, this);

    auto   defaultVirtualServer = new VirtualServer(config, indexInConfig);
    _virtualServers.insert(make_pair("server_name", defaultVirtualServer));

    initContentTypes();

    showDebugMessage("WebServer: Joining to EventSelector...");
    _eventSelector->add(this);
}

WebServer::WebServer(const WebServer &other)
    : FdHandler(other.getFd()), _port(other._port), _indexInConfig(other._indexInConfig),
    _eventSelector(other._eventSelector), _config(other._config), _timeoutMutex(other._timeoutMutex) {}

WebServer::~WebServer() {

    for (auto session : _sessions) {
        _eventSelector->remove(session);
    }
    _eventSelector->remove(this);
    _timeoutHandler.join();

//    for (std::list<WebSession*>::iterator it = _sessions.begin(); it != _sessions.end(); ++it) {
//        _eventSelector->remove((*it));
//    }
//    _eventSelector->remove(this);
}

bool WebServer::addNewVirtualServer(const Config &config, int serverIndexInConfig) {

    if (!config.isOptionDefined(serverIndexInConfig, "server_name")) {
        cerr << "Error: Field server_name is not defined for Server " << serverIndexInConfig << endl;
        return false;
    }

    string   serverName = config[to_string(serverIndexInConfig) + ".server.server_name"];
    if (serverName.empty()) {
        cerr << "Error: Field server_name is not defined for Server " << serverIndexInConfig << endl;
        return false;
    }

    auto   virtualServer = new VirtualServer(config, serverIndexInConfig);
    _virtualServers.insert(std::make_pair(serverName, virtualServer));

    return true;
}

void WebServer::checkSessionTimeout() {

    chrono::time_point<chrono::steady_clock> current_time;

    for (;;) {
        this_thread::sleep_for(chrono::milliseconds(10));
        _timeoutMutex.lock();
        current_time = chrono::high_resolution_clock::now();
        for (auto session_iter = _sessions.begin(); session_iter != _sessions.end(); ) {
            auto   tmp = session_iter;
            ++session_iter;
            int    duration = static_cast<int>(chrono::duration<float>(current_time - (*tmp)->getStartTime()).count());
            bool   not_waiting_response = !(*tmp)->wantBeWritten();
            if (duration > _timeoutSec && not_waiting_response) {
                showDebugMessage("WebSession " + to_string((*tmp)->getFd()) + ": Timeout");
                removeSession(*tmp);
            }
        }
        _timeoutMutex.unlock();
    }
}

WebServer *WebServer::init(EventSelector *eventSelector, int port, const Config &config, int indexInConfig) {

    int                  listenSocket, option, result;
    struct sockaddr_in   addr;

    showDebugMessage("WebServer: Creating socket...");
    listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket == -1) {
        return 0;
    }

    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    showDebugMessage("WebServer: Setting socket options...");
    option = 1;
    setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    showDebugMessage("WebServer: Binding socket...");
    result = ::bind(listenSocket, (struct sockaddr*)&addr, sizeof(addr));
    if (result == -1)
        return nullptr;

    result = listen(listenSocket, BACKLOG);
    if (result == -1)
        return nullptr;

    return new WebServer(eventSelector, port, listenSocket, config, indexInConfig);
}

void WebServer::handle(bool read, bool write) {

    if (!read)
        return;
    int                  clientSocket;
    struct sockaddr_in   addr;
    socklen_t            len = sizeof(addr);

    showDebugMessage("WebServer: Accepting new connection...");
    clientSocket = accept(this->getFd(), (struct sockaddr*)&addr, &len);
    if (clientSocket == -1)
        return;

    fcntl(clientSocket, F_SETFL, O_NONBLOCK);
    waitForDebug();

    auto   session = new WebSession(this, clientSocket);
    _sessions.push_front(session);
    _eventSelector->add(session);

    showDebugMessage("WebServer: Connection " + to_string(clientSocket) + " has been created");
}

void WebServer::removeSession(WebSession *session) {

//    this_thread::sleep_for(std::chrono::milliseconds(10));
    _eventSelector->remove(session);
    showDebugMessage("WebServer: WebSession "+ to_string(session->getFd()) +
                     " has been removed. Closing connection...");

    _sessions.erase(std::remove(_sessions.begin(), _sessions.end(), session), _sessions.end());

    showDebugMessage("WebServer: Deleting session...");
    delete session;

//    for (auto it = _sessions.begin(); it != _sessions.end(); ++it) {
//        if ((*it) == session) {
//            showDebugMessage("WebServer: WebSession "+ to_string(session->getFd()) +
//                                                                    " has been removed. Closing connection...");
//            delete *it;
//            _sessions.erase(it);
//            showDebugMessage("WebServer: Deleting session...");
//            return;
//        }
//    }
}

void WebServer::initContentTypes() {
    _contentTypes.insert(std::make_pair("aac", "audio/aac"));
    _contentTypes.insert(std::make_pair("abw", "application/x-abiword"));
    _contentTypes.insert(std::make_pair("arc", "application/x-freearc"));
    _contentTypes.insert(std::make_pair("avi", "video/x-msvideo"));
    _contentTypes.insert(std::make_pair("azw", "application/vnd.amazon.ebook"));
    _contentTypes.insert(std::make_pair("bin", "application/octet-stream"));
    _contentTypes.insert(std::make_pair("bmp", "image/bmp"));
    _contentTypes.insert(std::make_pair("bz", "application/x-bzip"));
    _contentTypes.insert(std::make_pair("bz2", "application/x-bzip2"));
    _contentTypes.insert(std::make_pair("csh", "application/x-csh"));
    _contentTypes.insert(std::make_pair("css", "text/css"));
    _contentTypes.insert(std::make_pair("csv", "text/csv"));
    _contentTypes.insert(std::make_pair("doc", "application/msword"));
    _contentTypes.insert(std::make_pair("docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document"));
    _contentTypes.insert(std::make_pair("eot", "application/vnd.ms-fontobject"));
    _contentTypes.insert(std::make_pair("epub", "application/epub+zip"));
    _contentTypes.insert(std::make_pair("gz", "application/gzip"));
    _contentTypes.insert(std::make_pair("gif", "image/gif"));
    _contentTypes.insert(std::make_pair("htm", "text/html"));
    _contentTypes.insert(std::make_pair("html", "text/html"));
    _contentTypes.insert(std::make_pair("ico", "image/vnd.microsoft.icon"));
    _contentTypes.insert(std::make_pair("ics", "text/calendar"));
    _contentTypes.insert(std::make_pair("jar", "application/java-archive"));
    _contentTypes.insert(std::make_pair("jpeg", "image/jpeg"));
    _contentTypes.insert(std::make_pair("jpg", "image/jpeg"));
    _contentTypes.insert(std::make_pair("js", "text/javascript"));
    _contentTypes.insert(std::make_pair("json", "application/json"));
    _contentTypes.insert(std::make_pair("jsonld", "application/ld+json"));
    _contentTypes.insert(std::make_pair("mid", "audio/midi audio/x-midi"));
    _contentTypes.insert(std::make_pair("midi", "audio/midi audio/x-midi"));
    _contentTypes.insert(std::make_pair("mjs", "text/javascript"));
    _contentTypes.insert(std::make_pair("mp3", "audio/mpeg"));
    _contentTypes.insert(std::make_pair("mpeg", "video/mpeg"));
    _contentTypes.insert(std::make_pair("mpkg", "application/vnd.apple.installer+xml"));
    _contentTypes.insert(std::make_pair("odp", "application/vnd.oasis.opendocument.presentation"));
    _contentTypes.insert(std::make_pair("ods", "application/vnd.oasis.opendocument.spreadsheet"));
    _contentTypes.insert(std::make_pair("odt", "application/vnd.oasis.opendocument.text"));
    _contentTypes.insert(std::make_pair("oga", "audio/ogg"));
    _contentTypes.insert(std::make_pair("ogv", "video/ogg"));
    _contentTypes.insert(std::make_pair("ogx", "application/ogg"));
    _contentTypes.insert(std::make_pair("opus", "audio/opus"));
    _contentTypes.insert(std::make_pair("otf", "font/otf"));
    _contentTypes.insert(std::make_pair("png", "image/png"));
    _contentTypes.insert(std::make_pair("pdf", "application/pdf"));
    _contentTypes.insert(std::make_pair("php", "application/php"));
    _contentTypes.insert(std::make_pair("ppt", "application/vnd.ms-powerpoint"));
    _contentTypes.insert(std::make_pair("pptx", "application/vnd.openxmlformats-officedocument.presentationml.presentation"));
    _contentTypes.insert(std::make_pair("rar", "application/vnd.rar"));
    _contentTypes.insert(std::make_pair("rtf", "application/rtf"));
    _contentTypes.insert(std::make_pair("sh", "application/x-sh"));
    _contentTypes.insert(std::make_pair("svg", "image/svg+xml"));
    _contentTypes.insert(std::make_pair("swf", "application/x-shockwave-flash"));
    _contentTypes.insert(std::make_pair("tar", "application/x-tar"));
    _contentTypes.insert(std::make_pair("tif", "image/tiff"));
    _contentTypes.insert(std::make_pair("tiff", "image/tiff"));
    _contentTypes.insert(std::make_pair("ts", "video/mp2t"));
    _contentTypes.insert(std::make_pair("ttf", "font/ttf"));
    _contentTypes.insert(std::make_pair("txt", "text/plain"));
    _contentTypes.insert(std::make_pair("vsd", "application/vnd.visio"));
    _contentTypes.insert(std::make_pair("wav", "audio/wav"));
    _contentTypes.insert(std::make_pair("weba", "audio/webm"));
    _contentTypes.insert(std::make_pair("webm", "video/webm"));
    _contentTypes.insert(std::make_pair("webp", "image/webp"));
    _contentTypes.insert(std::make_pair("woff", "font/woff"));
    _contentTypes.insert(std::make_pair("woff2", "font/woff2"));
    _contentTypes.insert(std::make_pair("xhtml", "application/xhtml+xml"));
    _contentTypes.insert(std::make_pair("xls", "application/vnd.ms-excel"));
    _contentTypes.insert(std::make_pair("xlsx", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"));
    _contentTypes.insert(std::make_pair("xml", "text/xml"));
    _contentTypes.insert(std::make_pair("xul", "application/vnd.mozilla.xul+xml"));
    _contentTypes.insert(std::make_pair("zip", "application/zip"));
    _contentTypes.insert(std::make_pair("7z", "application/x-7z-compressed"));
}

const Location *WebServer::getLocation(const string &serverName, const string &url) const {

    auto   virtualServer = _virtualServers.find(serverName);
    if (virtualServer == _virtualServers.end()) {
        auto   defaultServer = _virtualServers.begin()->second;

        return defaultServer->getLocation(url);
    }

    return virtualServer->second->getLocation(url);
}

int WebServer::getPort() const { return _port; }

const map<string, string> & WebServer::getContentTypes() const { return _contentTypes; }