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
#include <mutex>
#include <functional>
#include <thread>

#include "FdHandler.hpp"
#include "EventSelector.hpp"
#include "Location.hpp"
#include "WebSession.hpp"
#include "Utils.hpp"
#include "Config.hpp"
#include "VirtualServer.hpp"

using std::string;
using std::list;
using std::map;
using std::thread;
using std::mutex;

enum {
    BACKLOG = 16
};

class WebSession;

class WebServer : public FdHandler {

public:

    WebServer(const WebServer &other);
    ~WebServer();

    static WebServer*            init(EventSelector *eventSelector, int port, const Config &config, int indexInConfig);

    void                         removeSession(WebSession *session);
    int                          getPort() const;
    bool                         addNewVirtualServer(const Config &config, int serverIndexInConfig);
    const map<string, string>&   getContentTypes() const;
    const Location*              getLocation(const string &serverName, const string& url) const;

private:

    WebServer(EventSelector *eventSelector, int port, int fd, const Config &config, int indexInConfig);

    void   handle(bool read, bool write) override;
    void   checkSessionTimeout();
    void   initContentTypes();

    int                           _port;
    int                           _indexInConfig;
    const Config&                 _config;
    EventSelector*                _eventSelector;
    map<string, string>           _contentTypes;
    map<string, VirtualServer*>   _virtualServers;
    list<WebSession*>             _sessions;
    int                           _timeoutSec;
    thread                        _timeoutHandler;
    mutex&                        _timeoutMutex;


};

void showDebugMessage(const string& message);
void waitForDebug();

#endif //WEBSERV_WEBSERVER_HPP