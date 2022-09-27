//
// Created by Carmel Shells on 7/13/22.
//

#include "Location.hpp"

Location::Location(const Config &config, int serverIndexInConfig, const string &url)
        : _config(config), _serverIndexInConfig(serverIndexInConfig), _serverName(""),
        _root(""), _url(url), _returnValue(""), _cgiPath(""), _cgiName(""),
        _clientBodySize(global::MAX_CLIENT_MSG_SIZE), _autoindex(false) {

    showDebugMessage("Location " + url);

    // common options
    initSingleTrait(_serverName, "server_name");
//    initSingleTrait(_listen_addr, "listen");
    initSingleTrait(_root, "root");
    initSingleTrait(_autoindex, "autoindex", Utils::strToBool);
    initSingleTrait(_clientBodySize, "client_body_size", Utils::strToLongLong);
    initMultipleTrait(_methodsAllowed, "methods");
    initMultipleTrait(_indexNames, "index");

    // location options
    initSingleTrait(_root, "location " + _url + ".root");
    initSingleTrait(_returnValue, "location " + _url + ".return");
    initSingleTrait(_cgiPath, "location " + _url + ".cgi_path");
    initSingleTrait(_cgiName, "location " + _url + ".cgi");
    initSingleTrait(_clientBodySize, "location " + _url + ".client_body_size", Utils::strToLongLong);
    initSingleTrait(_autoindex, "location " + _url + ".autoindex", Utils::strToBool);
    initMultipleTrait(_indexNames, "location " + _url + ".index");
    initMultipleTrait(_methodsAllowed, "location " + _url + ".methods");

    showDebugMessage("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");
}

Location::~Location() {}

void Location::initSingleTrait(string &trait, const string &optionName) {
    if (_config.isOptionDefined(_serverIndexInConfig, optionName)) {
        trait = _config[to_string(_serverIndexInConfig) + ".server." + optionName];
    }
}

template <class T>
void Location::initSingleTrait(T &trait, const string &optionName, T (*strToT)(const string &str)) {

    if (_config.isOptionDefined(_serverIndexInConfig, optionName)) {
        try {
            trait = strToT(_config[to_string(_serverIndexInConfig) + ".server." + optionName]);
        } catch (std::exception&) {
            trait = T();
        }
    }
}

void Location::initMultipleTrait(list<string> &optionList, const string &optionName) {
    if (!_config.isOptionDefined(_serverIndexInConfig, optionName))
    {
        std::cout << "optionName: " << optionName << "$ return" << std::endl;
        return;
    }

    std::cout << "optionName: " << optionName << "$" << std::endl;
    optionList.clear();
    vector<string>    options = Utils::split(_config[to_string(_serverIndexInConfig) + ".server." + optionName], ',');

    for (auto it = options.begin(); it != options.end(); ++it) {
        optionList.push_front(*it);
        std::cout << "option: " << *it << "$" << std::endl;
    }
}

const string &Location::getServerName() const { return _serverName; }

const std::string &Location::getRoot() const { return _root; }

long long Location::getClientBodySize() const { return _clientBodySize; }

const list <string> Location::getIndexNames() const { return _indexNames; }

const list <string> Location::getMethodsAllowed() const { return _methodsAllowed; }

const std::string &Location::getUrl() const { return _url; }

const string &Location::getReturnValue() const { return _returnValue; }

std::string Location::getAbsolutePath() const { return getRoot() + _url; }

const string &Location::getCgiPath() const { return _cgiPath; }

const string &Location::getCgiName() const { return _cgiName; }

bool Location::isAutoindex() const { return _autoindex; }

bool Location::isCgiSupport() const { return !_cgiPath.empty() && !_cgiName.empty(); }

bool Location::isMethodAllowed(const std::string& method) const {
    for (list<string>::const_iterator it = _methodsAllowed.begin(); it != _methodsAllowed.end(); ++it) {
        if (*it == method)
            return true;
    }
    return false;
}

void Location::setServerName(const string &name) { _serverName = name; }

void Location::setRoot(const std::string &root) { _root = root; }

void Location::setClientBodySize(long long clientBodySize) { _clientBodySize = clientBodySize; }

void Location::setIndexNames(const list<string> &indexNames) { _indexNames = indexNames; }

void Location::setMethodsAllowed(const list<string> &methodsAllowed) { _methodsAllowed = methodsAllowed; }

void Location::setAutoindex(bool value) { _autoindex = value; }

void Location::setUrl(const std::string &url) { _url = url; }