/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpMessage.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cshells <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/31 16:06:08 by cshells           #+#    #+#             */
/*   Updated: 2022/01/31 16:06:09 by cshells          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpMessage.hpp"

HttpMessage::HttpMessage() {}

HttpMessage::~HttpMessage() {}

HttpMessage::HttpMessage(const HttpMessage &other) {}

HttpMessage &HttpMessage::operator=(const HttpMessage &other) {}


//void HttpMessage::setHeaderLines(const std::vector<std::string> &headerLines) { _headerLines = headerLines; }

bool HttpMessage::addHeaderParam(const string &key, const string &value) {
    return _header_params.insert(make_pair(key, value)).second;
}

bool HttpMessage::removeHeaderParam(const string &key) {
    return _header_params.erase(key);
}

void HttpMessage::setData(const char *begin, const char *end) {
    if (begin && end - 1)
        _data.assign(begin, end);
    else
        std::cout << "HttpMessage: Data setting error" << std::endl;
}

void HttpMessage::appendData(const char *begin, const char *end) {
    if (begin && end - 1)
        _data.insert(_data.end(), begin, end);
    else
        std::cout << "HttpMessage: Data appending error" << std::endl;
}

std::vector<std::string> HttpMessage::getHeaderLines() const { return _headerLines; }

const std::vector<char> &HttpMessage::getData() const { return _data; }

const string& HttpMessage::getHeaderParamValue(const string &key) const {

    map<string, string>::const_iterator it = _header_params.find(key);

    if (it != _header_params.end())
        return it->second;

    return "";
}
