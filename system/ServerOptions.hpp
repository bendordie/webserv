/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerOptions.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cshells <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/11 14:48:32 by cshells           #+#    #+#             */
/*   Updated: 2022/03/11 14:48:34 by cshells          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef __SERVERTRAITS_HPP__
#define __SERVERTRAITS_HPP__

#include <list>
#include <string>

#include "Config.hpp"
#include "Utils.hpp"
//#include "VirtualServer.hpp"

using std::string;
using std::list;


class ServerOptions {

    friend class WebServer;

public:

    ServerOptions(const Config &config, int index_in_config);
    virtual ~ServerOptions();

    const string&        getServerName() const;
    const string&        getRoot() const;
    long long            getClientBodySize() const;
    const list<string>   getIndexList() const;
    const list<string>   getMethodsList() const;
    bool                 isAutoindex() const;
    bool                 isMethodAllowed(const string &method) const;

    void                 setServerName(const string &name);
    void                 setRoot(const string &root);
    void                 setClientBodySize(long long client_body_size);
    void                 setIndexList(const list<string> &index_list);
    void                 setMethodsList(const list<string> &methods_list);
    void                 setAutoindex(bool value);

private:


    const Config    &_config;
    int             _indexInConfig;
    string          _server_name;

protected:

    void                initSingleTrait(string &trait, const string &trait_name);
    template <class T>
    void                initSingleTrait(T &trait, const string &trait_name, T (*strToT)(const string &str));
    void                initMultipleTrait(list<string> &trait_list, const string &trait_name);

    string          _root;
    long long       _client_body_size;
    bool            _autoindex;
    list<string>    _index_list;
    list<string>    _methods_list;


};

#endif //WEBSERV_SERVERTRAITS_HPP
