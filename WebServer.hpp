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
#include "Utils.hpp"
#include "Config.hpp"

using namespace std;



class WebSession;

class ServerTraits;

class WebServer : public FdHandler {

public:

    ~WebServer();

    static WebServer*           start(EventSelector *event_selector, int port, const Config &config);
    void                        removeSession(WebSession *session);

//    const list<string>&         getIndexList() const;
//    string                      getUri(const string& path_requested) const;
//    const list<Location*>       getLocationList() const;
    const map<string, string>   getContentTypes() const;

    static int  id_generator;


public:

    class ServerTraits {

        friend class WebServer;

        virtual ~ServerTraits() {}

    public:

        const string&       getRoot() const { return _root; }
        long long           getClientBodySize() const { return _client_body_size; }
        const list<string>  getIndexList() const { return _index_list; }
        const list<string>  getMethodsList() const { return _methods_list; }
        bool                isAutoindex() const { return _autoindex; }
        void                setRoot(const string &root) { _root = root; }
        void                setClientBodySize(long long client_body_size) { _client_body_size = client_body_size; }
        void                setIndexList(const list<string> &index_list) { _index_list = index_list; }
        void                setMethodsList(const list<string> &methods_list) { _methods_list = methods_list; }
        void                setAutoindex(bool value) { _autoindex = value; }

    private:

        ServerTraits() {};

        string          _root;
        long long       _client_body_size;
        bool            _autoindex;
        list<string>    _index_list;
        list<string>    _methods_list;

//        void                removeLastSlashInRoot();

    };

    class LocationTraits : public ServerTraits {

        friend class WebServer;

    public:

        LocationTraits(const ServerTraits &serv_traits, const string &url)
        : ServerTraits(serv_traits), _url(url) {};

        const string&   getUrl() const { return _url; }
        string          getAbsolutePath() const { std::cout << "ROOT: " << _root << std::endl;
            std::cout << "URL: " << _url << std::endl; return _root + _url; }

        void            setUrl(const string &url) { _url = url; }

    private:

        string      _url;
    };

public:

    const WebServer::ServerTraits*       getLocationTraits(const string& uri) const;

private:

    WebServer(EventSelector *event_selector, int fd, const Config &config);

    virtual void        Handle(bool read, bool write);
    void                initServerTraits();
    void                initLocations();
    void                initContentTypes();
    template <class T>
    void                initSingleTrait(T &trait, const string &trait_name, T (*strToT)(const string &str));
    void                initSingleTrait(string &trait, const string &trait_name);
    void                initMultipleTrait(list<string> &trait_list, const string &trait_name);

    EventSelector           *_event_selector;
    list<WebSession*>       _session_list;
    int                     _id;
    string                  _display_name;
    string                  _listen_addr;
    string                  _server_name;
    ServerTraits            *_traits;
    list<LocationTraits*>   _location_list;
    const Config            &_config;

    map<string, string>     _content_types;



};

#endif //WEBSERV_WEBSERVER_HPP
