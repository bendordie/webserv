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

//HttpMessage::HttpMessage(const char *rawDataBegin, const char *rawDataEnd) {
//    _protocol = findHttpProtocolFromBuffer(rawDataBegin);
//    vector<string> headerEntries = Utils::split(string(rawDataBegin, rawDataEnd), "\r\n");
//
//    for (auto entry : headerEntries) {
//        const string&   key = entry.substr(0)
//    }
//}

HttpMessage::HttpMessage(const string &protocol) : _protocol(protocol), _data(nullptr), _dataSize(0) {}

HttpMessage::~HttpMessage() {
    delete [] _data;
}

void HttpMessage::setData(const char *begin, const char *end) {
    showDebugMessage("HttpMessage: Setting data...");

    if (begin && end - 1) {
        if (_data)
            delete [] _data;
        size_t  size = end - begin;
        showDebugMessage("HttpMessage: Data size: " + std::to_string(size));
        _data = new char[size];
        memcpy(_data, begin, size);
        _dataSize = size;
    }
    else
        std::cerr << "HttpMessage: Data setting error" << std::endl;
}

void HttpMessage::setChunkedData(const char *begin, const char *end, bool last_chunk) {
    showDebugMessage("HttpMessage: Preparing data chunk setting...");

    if (begin == end)
    {
        showDebugMessage("HttpMessage: There is noe data to transfer");

        char*    tmp = new char[_dataSize + 5];

        memcpy(tmp, _data, _dataSize);
        memcpy(tmp + _dataSize, "0\r\n\r\n", 5);
        _dataSize += 5;

        delete [] _data;
        _data = tmp;
        return;
    }

    if (begin && end - 1) {

        if (_data)
            delete [] _data;

        size_t  size = end - begin;

        showDebugMessage("HttpMessage: Chunk size: " + std::to_string(size));

        size_t  tail_len = last_chunk ? strlen("\r\n0\r\n\r\n") : strlen("\r\n");  // 7 => \r\n0\r\n\r\n, 2 => \r\n
        string  chunk_size = Utils::intToHexString(size) + "\r\n";

        showDebugMessage("HttpMessage: Chunk size in string: " + chunk_size);

        _data = new char[chunk_size.length() + size + tail_len];
        memcpy(_data, chunk_size.c_str(), chunk_size.length());
        _dataSize += chunk_size.length();
        memcpy(_data + _dataSize, begin, size);
        _dataSize += size;
        memcpy(_data + _dataSize, "\r\n0\r\n\r\n", tail_len);
        _dataSize += tail_len;
    }
    else
        std::cerr << "HttpMessage: Data setting error" << std::endl;
}

void HttpMessage::appendData(const char *begin, const char *end) {
    if (!_data) {
        setData(begin, end);
        return;
    }
    if (begin && end - 1) {
        size_t size = end - begin;
        char   *tmp = new char[_dataSize + size];
        memcpy(tmp, _data, _dataSize);
        memcpy(tmp + _dataSize, begin, size);
        delete [] _data;
        _data = tmp;
        _dataSize += size;
    }
    else
        std::cerr << "HttpMessage: Data appending error" << std::endl;
}

void HttpMessage::appendChunkedData(const char *begin, const char *end, bool last_chunk) {
    showDebugMessage("HttpMessage: Preparing data chunk appending...");

    if (begin == end)
    {
        char*    tmp = new char[_dataSize + 5];

        memcpy(tmp, _data, _dataSize);
        memcpy(tmp + _dataSize, "0\r\n\r\n", 5);
        _dataSize += 5;

        delete [] _data;
        _data = tmp;
        return;
    }

    if (!_data) {
        setChunkedData(begin, end, last_chunk);
        return;
    }
    if (begin && end - 1) {
        size_t   size = end - begin;

        showDebugMessage("HttpMessage: Data size to chunk appending: " + std::to_string(size));

        string   chunk_size = Utils::intToHexString(size) + "\r\n";
        size_t   tail_len = last_chunk ? strlen("\r\n0\r\n\r\n") : strlen("\r\n");  // 7 => \r\n0\r\n\r\n, 2 => \r\n
        char*    tmp = new char[_dataSize + chunk_size.length() + size + tail_len];

        memcpy(tmp, _data, _dataSize);
        memcpy(tmp + _dataSize, chunk_size.c_str(), chunk_size.length());
        _dataSize += chunk_size.length();
        memcpy(tmp + _dataSize, begin, size);
        _dataSize += size;
        memcpy(tmp + _dataSize, "\r\n0\r\n\r\n", tail_len);
        _dataSize += tail_len;

        delete [] _data;
        _data = tmp;
    }
    else
        std::cerr << "HttpMessage: Data appending error" << std::endl;
}

bool HttpMessage::addHeaderEntry(const string &key, const string &value) {
    return _headerEntries.insert(make_pair(key, value)).second;
}

bool HttpMessage::addHeaderEntry(const pair<string, string> &entry) {
    return _headerEntries.insert(entry).second;
}

bool HttpMessage::removeHeaderEntry(const string &key) {
    return _headerEntries.erase(key);
}

void HttpMessage::setHeaderEntries(const map<string, string> &headerEntries) { _headerEntries = headerEntries; }

const string &HttpMessage::getProtocol() const { return _protocol; }

const char *HttpMessage::getData() const { return _data; }

size_t HttpMessage::getDataSize() const { return _dataSize; }

string HttpMessage::getHeaderEntryValue(const string &key) const {

    map<string, string>::const_iterator it = _headerEntries.find(key);

    if (it != _headerEntries.end())
        return it->second;

    return "";
}

string HttpMessage::operator[](const string &key) const {
    return getHeaderEntryValue(key);
}

const char *HttpMessage::findMessageEnd(const char *msgBegin) {

    const char*   msgEnd;

    msgEnd = strstr(msgBegin, "\r\n\r\n");
    if (msgEnd) {
        showDebugMessage("HttpMessage: Message end exists");
        msgEnd += strlen("\r\n\r\n");
    }
    else
        showDebugMessage("HttpMessage: Message end not found");

    return msgEnd;
}

std::string HttpMessage::getOptionValueFromBuffer(const char *buffer, const char *optionName) {

    const char*   optionBegin;
    const char*   separatorBegin;
    const char*   optionValueEnd;
    size_t        optionLen = strlen(optionName);

    if (!buffer || !optionName)
        return "";

    optionBegin = strstr(buffer, optionName);
    if (!optionBegin || !(optionBegin + optionLen))
        return "";
    separatorBegin = strstr(optionBegin, ": ");
    if (!separatorBegin)
        return "";
    optionValueEnd = strstr(optionBegin + optionLen, "\r\n");
    if (!optionValueEnd)
        return "";

    return string(separatorBegin + strlen(": "), optionValueEnd);
}

string HttpMessage::findHttpProtocolFromBuffer(const char *bufferBegin) {
    const char*   begin = strstr(bufferBegin, "HTTP/");

    if (!begin)
        return "";

    string   protocol = string(begin, strlen("HTTP/1.*"));

    if (protocol != "HTTP/1.0" && protocol != "HTTP/1.1")
        return "";

    return protocol;
}
