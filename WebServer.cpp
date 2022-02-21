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

WebServer::WebServer(EventSelector *event_selector, int fd)
: FdHandler(fd, true), _event_selector(event_selector) {

    std::cout << "WebServer: Joining to EventSelector..." << std::endl;
    _event_selector->add(this);
    _index_list.push_back("index.html");
}

WebServer::~WebServer() {

    for (std::list<WebSession*>::iterator it = _session_list.begin(); it != _session_list.end(); ++it) {
        _event_selector->remove((*it));
    }
    _event_selector->remove(this);
}

WebServer *WebServer::start(EventSelector *event_selector, int port) {

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

    return new WebServer(event_selector, listen_socket);
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

const list<Location *> WebServer::getLocationList() const { return _location_list; }

const map<string, string> WebServer::getContentTypes() const { return _content_types; }

const list<string> &WebServer::getIndexList() const { return _index_list; }