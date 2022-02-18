/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cshells <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/02 15:26:22 by cshells           #+#    #+#             */
/*   Updated: 2022/02/02 15:26:23 by cshells          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef __WEBSERVER_HPP__
#define __WEBSERVER_HPP__

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <list>
#include <map>
#include "FdHandler.hpp"
#include "EventSelector.hpp"
#include "WebSession.hpp"
#include "Location.hpp"
#include "Utils.hpp"

using namespace std;



class WebSession;

class WebServer : public FdHandler {

public:

    ~WebServer();

    static WebServer*           start(EventSelector *event_selector, int port);
    void                        removeSession(WebSession *session);
    const list<string>&         getIndexList() const;
    const list<Location*>       getLocationList() const;
    const map<string, string>   getContentTypes() const;


private:

    WebServer(EventSelector *event_selector, int fd);

    virtual void        Handle(bool read, bool write);

    EventSelector       *_event_selector;
    list<WebSession*>   _session_list;
    string              _default_root;
    string              _server_name;
    long long           _client_body_size;
    list<string>        _index_list;
    list<Location*>     _location_list;
    map<string, string> _content_types;


};

#endif //WEBSERV_WEBSERVER_HPP
