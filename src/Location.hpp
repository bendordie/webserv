//
// Created by Carmel Shells on 7/13/22.
//

#ifndef WEBSERV_LOCATION_HPP
#define WEBSERV_LOCATION_HPP

#include <iostream>
#include <list>
#include <string>
#include "global_variables.hpp"
#include "Config.hpp"
#include "Utils.hpp"

class Location {

    friend class WebServer;
    friend class VirtualServer;

public:

    Location(const Config& config, int serverIndexInConfig, const string& url);
    ~Location();

    const string&        getServerName() const;
    const string&        getRoot() const;
    long long            getClientBodySize() const;
    const list<string>   getIndexNames() const;
    const list<string>   getMethodsAllowed() const;
    const string&        getUrl() const;
    const string&        getReturnValue() const;
    string               getAbsolutePath() const;
    const string&        getCgiPath() const;
    const string&        getCgiName() const;
    bool                 isAutoindex() const;
    bool                 isCgiSupport() const;
    bool                 isMethodAllowed(const string& method) const;

    void                 setServerName(const string& name);
    void                 setRoot(const string& root);
    void                 setUrl(const string& url);
    void                 setClientBodySize(long long clientBodySize);
    void                 setIndexNames(const list<string>& indexNames);
    void                 setMethodsAllowed(const list<string>& methodsAllowed);
    void                 setAutoindex(bool value);

private:

    void                initSingleTrait(string& option, const string& optionName);
    template <class T>
    void                initSingleTrait(T& option, const string& optionName, T (*strToT)(const string& str));
    void                initMultipleTrait(list<string>& optionList, const string& optionName);


    const Config&   _config;
    int             _serverIndexInConfig;
    string          _serverName;

    string          _root;
    string          _url;
    string          _returnValue;
    string          _cgiPath;
    string          _cgiName;
    long long       _clientBodySize;
    bool            _autoindex;
    list<string>    _indexNames;
    list<string>    _methodsAllowed;





};

#endif //WEBSERV_LOCATION_HPP
