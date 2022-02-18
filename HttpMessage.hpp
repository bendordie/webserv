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

    vector<string>              getHeaderLines() const;
    const vector<char>&         getData() const;
    const string&               getHeaderParamValue(const string &key) const;


    bool                        addHeaderParam(const string &key, const string &value);
    bool                        removeHeaderParam(const string &key);
    void                        setData(const char *begin, const char *end);
    void                        appendData(const char *begin, const char *end);
    void                        setHeaderLines(const std::vector<std::string> &headerLines);


protected:

    map<string, string> _header_params;
    vector<string>      _headerLines;
    vector<char>        _data;



};


#endif //WEBSERV_HTTPMESSAGE_HPP
