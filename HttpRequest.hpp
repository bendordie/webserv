/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cshells <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/31 16:06:15 by cshells           #+#    #+#             */
/*   Updated: 2022/01/31 16:06:16 by cshells          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __HTTPREQUEST_HPP__
#define __HTTPREQUEST_HPP__
#pragma once

#include <cstring>
#include <cstdlib>
#include <vector>
#include "HttpMessage.hpp"
#include "Utils.hpp"

using namespace std;

class HttpRequest : public HttpMessage {


public:

    HttpRequest(string type, string path);
    HttpRequest(string type, string path, bool keep_alive,
                string content_type, long long int content_length,
                size_t header_size, size_t size);
//    HttpRequest(const char *header_begin, const char *header_last);
//    HttpRequest(const char *header_begin, size_t size);
//    HttpRequest(std::vector<std::string> header_lines);
//    HttpRequest(std::string type);
    ~HttpRequest();

    static HttpRequest*         createRequest(string type, const char *header_begin,
                                              const char *header_end, size_t buf_size);


    static const char*          defineHeaderEnd(const char *begin);
    static string               defineRequestPath(const char *begin);
    static string               extractParam(const char *header_begin, const char *param_name);
    static pair<string, string> extractParam2(const string &str);
    static void                 defineHeaderParams(bool keep_alive, long long int &content_length, string &content_type);


    const string&               getType() const;
    const string&               getPath() const;
    const string&               getContentType() const;
    const long long&            getContentLength() const;
    const size_t&               getHeaderSize() const;
    const size_t&               getSize() const;
    const size_t&               getDataSize() const;
    const bool&                 getFullReceivedFlag() const;
    const size_t                getRestDataSize() const;
    const string&               getUri() const;
    bool                        isKeepAlive() const;
//    virtual std::vector<std::string>    getHeaderLines() const;
//    virtual const std::vector<uint8_t>& getData() const;

    void                        setPath(const string &path);
    void                        setContentType(const string &contentType);
    void                        setContentLength(const long long int &contentLength);
    void                        setUri(const string &uri);
    void                        setFullReceivedFlag(bool value);

protected:

    HttpRequest();

private:

    string          _type;
    string          _path;
    string          _content_type;
    long long       _content_length;
    string          _uri;
    bool            _keep_alive;
    size_t          _header_size;
    size_t          _size;
    bool            _full_received;

};


#endif //WEBSERV_HTTPREQUEST_HPP
