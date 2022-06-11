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

HttpMessage::HttpMessage(const string &protocol) : _protocol(protocol), _data(0), _data_size(0) {}

HttpMessage::~HttpMessage() {}

HttpMessage::HttpMessage(const HttpMessage &other) {}

HttpMessage &HttpMessage::operator=(const HttpMessage &other) {}

void HttpMessage::setData(const char *begin, const char *end) {
    if (begin && end - 1) {
        if (_data)
            delete [] _data;
        size_t  size = end - begin;
        _data = new char[size];
        memcpy(_data, begin, size);
        _data_size = size;
    }
    else
        std::cout << "HttpMessage: Data setting error" << std::endl;
}

void HttpMessage::setChunkedData(const char *begin, const char *end, bool last_chunk) {
    std::cout << "HttpMessage: Preparing data chunk encoding..." << std::endl;

    if (begin && end - 1) {
        if (_data)
            delete [] _data;
        size_t  size = end - begin;
        size_t  tail_len = last_chunk ? strlen("\r\n0\r\n\r\n") : strlen("\r\n");  // 7 => \r\n0\r\n\r\n, 2 => \r\n
        string  chunk_size = Utils::intToHexString(size) + "\r\n";
        _data = new char[chunk_size.length() + size + tail_len];
        memcpy(_data, chunk_size.c_str(), chunk_size.length());
        memcpy(_data, begin, size);
        _data_size = chunk_size.length() + size;
        memcpy(_data + _data_size, "\r\n0\r\n\r\n", tail_len);
        _data_size += tail_len;
    }
    else
        std::cout << "HttpMessage: Data setting error" << std::endl;
}

void HttpMessage::appendData(const char *begin, const char *end) {
    if (!_data) {
        setData(begin, end);
        return;
    }
    if (begin && end - 1) {
        size_t size = end - begin;
        char   *tmp = new char[_data_size + size];
        memcpy(tmp, _data, _data_size);
        memcpy(tmp + _data_size, begin, size);
        delete [] _data;
        _data = tmp;
        _data_size += size;
    }
    else
        std::cout << "HttpMessage: Data appending error" << std::endl;
}

void HttpMessage::appendChunkedData(const char *begin, const char *end, bool last_chunk) {
    std::cout << "HttpMessage: Preparing data chunk encoding..." << std::endl;

    if (!_data) {
        setChunkedData(begin, end, last_chunk);
        return;
    }
    if (begin && end - 1) {
        size_t  size = end - begin;
        string  chunk_size = Utils::intToHexString(size) + "\r\n";
        size_t  tail_len = last_chunk ? strlen("\r\n0\r\n\r\n") : strlen("\r\n");  // 7 => \r\n0\r\n\r\n, 2 => \r\n
        char   *tmp = new char[_data_size + chunk_size.length() + size + tail_len];
        memcpy(tmp, _data, _data_size);
        memcpy(tmp + _data_size, chunk_size.c_str(), chunk_size.length());
        _data_size += chunk_size.length();
        memcpy(tmp + _data_size, begin, size);
        _data_size += size;
        memcpy(tmp + _data_size, "\r\n0\r\n\r\n", tail_len);
        _data_size += tail_len;

//        memcpy(tmp + _data_size, "\r\n", 2);
//        memcpy(tmp + _data_size + 2, 0, 1);
//        memcpy(tmp + _data_size + 3, "\r\n\r\n", 4);

        delete [] _data;
        _data = tmp;
//        std::cout << "_data[last]:" << (int)_data[_data_size-1] << std::endl;
//        std::cout << "_data[last-1]:" << (int)_data[_data_size-2] << std::endl;
//        std::cout << "_data[last-2]:" << (int)_data[_data_size-3] << std::endl;
//        std::cout << "_data[last-3]:" << (int)_data[_data_size-4] << std::endl;
//        std::cout << "_data[last-4]:" << (int)_data[_data_size-5] << std::endl;
//        std::cout << "_data[last-5]:" << (int)_data[_data_size-6] << std::endl;
    }
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

const string &HttpMessage::getProtocol() const { return _protocol; }

const char *HttpMessage::getData() const { return _data; }

size_t HttpMessage::getDataSize() const { return _data_size; }

string HttpMessage::getHeaderEntryValue(const string &key) const {

    map<string, string>::const_iterator it = _header_entries.find(key);

    if (it != _header_entries.end())
        return it->second;

    return "";
}

string HttpMessage::operator[](const string &key) const {
    return getHeaderEntryValue(key);
}

const char *HttpMessage::findMessageEnd(const char *msg_begin) {
    std::cout << "HttpMessage: Defining http message end..." << std::endl;

    const char    *msg_end;
    
    msg_end = strstr(msg_begin, "\r\n\r\n");
    if (msg_end) {
        std::cout << "HttpMessage: Message end exists" << std::endl;
        msg_end += strlen("\r\n\r\n");
    }
    else
        std::cout << "HttpMessage: Message end not found" << std::endl;

    return msg_end;
}
