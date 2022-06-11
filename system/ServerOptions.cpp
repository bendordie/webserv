/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerOptions.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cshells <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/11 14:48:37 by cshells           #+#    #+#             */
/*   Updated: 2022/03/11 14:48:39 by cshells          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerOptions.hpp"

ServerOptions::ServerOptions(const Config &config, int index_in_config)
    : _config(config), _indexInConfig(index_in_config){

    initSingleTrait(_server_name, "server_name");
//    initSingleTrait(_listen_addr, "listen");
    initSingleTrait(_root, "root");
    initSingleTrait(_autoindex, "autoindex", Utils::strToBool);
    initSingleTrait(_client_body_size, "client_body_size", Utils::strToLongLong);
    initMultipleTrait(_methods_list, "methods");
    initMultipleTrait(_index_list, "index");
}

ServerOptions::~ServerOptions() {}

void ServerOptions::initSingleTrait(string &trait, const string &trait_name) {
    if (_config.isTraitDefined(_indexInConfig, trait_name)) {
        trait = _config[to_string(_indexInConfig) + ".server." + trait_name];
    }
}

template <class T>
void ServerOptions::initSingleTrait(T &trait, const string &trait_name, T (*strToT)(const string &str)) {

    if (_config.isTraitDefined(_indexInConfig, trait_name)) {
        try {
            trait = strToT(_config[to_string(_indexInConfig) + ".server." + trait_name]);
        } catch (std::exception&) {
            trait = T();
        }
    }
}

void ServerOptions::initMultipleTrait(list<string> &trait_list, const string &trait_name) {
    if (!_config.isTraitDefined(_indexInConfig, trait_name))
    {
        std::cout << "trait_name: " << trait_name << "$ return" << std::endl;
        return;
    }
    trait_list.clear();
    vector<string>    traits = Utils::split(_config[to_string(_indexInConfig) + ".server." + trait_name], ',');

    for (auto it = traits.begin(); it != traits.end(); ++it) {
        trait_list.push_front(*it);
    }
}

const string &ServerOptions::getServerName() const { return _server_name; }

const std::string &ServerOptions::getRoot() const { return _root; }

long long ServerOptions::getClientBodySize() const { return _client_body_size; }

const list <string> ServerOptions::getIndexList() const { return _index_list; }

const list <string> ServerOptions::getMethodsList() const { return _methods_list; }

bool ServerOptions::isAutoindex() const { return _autoindex; }

bool ServerOptions::isMethodAllowed(const std::string &method) const {
    for (list<string>::const_iterator it = _methods_list.begin(); it != _methods_list.end(); ++it) {
        if (*it == method)
            return true;
    }
    return false;
}

void ServerOptions::setServerName(const string &name) { _server_name = name; }

void ServerOptions::setRoot(const std::string &root) { _root = root; }

void ServerOptions::setClientBodySize(long long client_body_size) { _client_body_size = client_body_size; }

void ServerOptions::setIndexList(const list <string> &index_list) { _index_list = index_list; }

void ServerOptions::setMethodsList(const list <string> &methods_list) { _methods_list = methods_list; }

void ServerOptions::setAutoindex(bool value) { _autoindex = value; }
