/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpMessage.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cshells <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/31 16:06:03 by cshells           #+#    #+#             */
/*   Updated: 2022/01/31 16:06:04 by cshells          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __HTTPMESSAGE_HPP__
#define __HTTPMESSAGE_HPP__
#pragma once

#include <unistd.h>
#include <iostream>
#include <vector>
#include <map>

#include "Utils.hpp"

using std::string;
using std::pair;

class HttpMessage {


public:

    HttpMessage(const string &protocol);
    HttpMessage(const char* rawDataBegin, const char* rawDataEnd);
    HttpMessage(const char* rawData, size_t rawDataSize);
    virtual ~HttpMessage();
    HttpMessage(const HttpMessage &other);
    HttpMessage& operator=(const HttpMessage &other);

    const string&        getProtocol() const;
    string               getHeaderEntryValue(const string &key) const;
    const char*          getData() const;
    size_t               getDataSize() const;
    string               operator[](const string &key) const;

    void                 setData(const char *begin, const char *end);
    void                 setChunkedData(const char *begin, const char *end, bool last_chunk = false);
    void                 appendData(const char *begin, const char *end);
    void                 appendChunkedData(const char *begin, const char *end, bool last_chunk = false);
    bool                 addHeaderEntry(const string &key, const string &value);
    bool                 addHeaderEntry(const pair<string, string> &entry);
    bool                 removeHeaderEntry(const string &key);
    void                 setHeaderEntries(const map<string, string> &header_entries);

    static const char*   findMessageEnd(const char *msgBegin);
    static string        findHttpProtocolFromBuffer(const char* bufferBegin);
    static string        getOptionValueFromBuffer(const char *buffer, const char *optionName);



protected:

    string                _protocol;
    map<string, string>   _headerEntries;
    char*                 _data;
    size_t                _dataSize;

};

void showDebugMessage(const string& message);

#endif //WEBSERV_HTTPMESSAGE_HPP
