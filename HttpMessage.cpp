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

bool HttpMessage::addHeaderEntry(const string &key, const string &value) {
    return _header_entries.insert(make_pair(key, value)).second;
}

bool HttpMessage::addHeaderEntry(const pair<string, string> &entry) {
    return _header_entries.insert(entry).second;
}

bool HttpMessage::removeHeaderEntry(const string &key) {
    return _header_entries.erase(key);
}

void HttpMessage::setHeaderEntries(const map<string, string> &header_entries) { _header_entries = header_entries; }

const std::vector<char> &HttpMessage::getData() const { return _data; }

string HttpMessage::getHeaderEntryValue(const string &key) const {

    map<string, string>::const_iterator it = _header_entries.find(key);

    if (it != _header_entries.end())
        return it->second;

    return "";
}

string HttpMessage::operator[](const string &key) const {
    return getHeaderEntryValue(key);
}
