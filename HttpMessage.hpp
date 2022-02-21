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

using namespace std;

class HttpMessage {


public:

    HttpMessage();
    virtual ~HttpMessage();
    HttpMessage(const HttpMessage &other);
    HttpMessage& operator=(const HttpMessage &other);

    const vector<char>&         getData() const;
    string                      getHeaderEntryValue(const string &key) const;
    string                      operator[](const string &key) const;

    void                        setData(const char *begin, const char *end);
    void                        appendData(const char *begin, const char *end);
    bool                        addHeaderEntry(const string &key, const string &value);
    bool                        addHeaderEntry(const pair<string, string> &entry);
    bool                        removeHeaderEntry(const string &key);
    void                        setHeaderEntries(const map<string, string> &header_entries);


protected:

    map<string, string> _header_entries;
    vector<char>        _data;



};


#endif //WEBSERV_HTTPMESSAGE_HPP
