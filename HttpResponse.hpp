/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cshells <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/31 19:40:16 by cshells           #+#    #+#             */
/*   Updated: 2022/01/31 19:40:18 by cshells          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef __HTTPRESPONSE_HPP__
#define __HTTPRESPONSE_HPP__

#include "WebServer.hpp"
#include "WebSession.hpp"
#include "HttpMessage.hpp"
#include "HttpRequest.hpp"

class HttpRequest;

class WebSession;

class WebServer;

class HttpResponse : public HttpMessage {


public:

    HttpResponse(int status_code, const string &status_msg);
    HttpResponse(int status_code, const string &status_msg, const string &connection,
                 const string &data_path, const map<string, string> &content_types);
    HttpResponse(int status_code, const string &status_msg, const string &connection,
                 const char *data, size_t data_size, string content_type = "");
    ~HttpResponse();

    static HttpResponse*    createResponse(HttpRequest *request, WebServer *server);
    static HttpResponse*    createGetResponse(HttpRequest *request, WebServer *server);
    static HttpResponse*    createPostResponse(HttpRequest *request, WebServer *server);
    static HttpResponse*    createDeleteResponse(HttpRequest *request, WebServer *server);
    void                    makeResponseHeader(const string &file_time);
    static string           makeAutoindexPage(const string &uri, const string &abs_path);
    static string           makeAutoindexLine(list<struct dirent>::iterator file, const string &path);
//    static HttpResponse*       createNoBodyResponse(int status_code, string status_msg, string body_path = "");

protected:


private:

    int             _status_code;
    string          _status_msg;
    string          _header;
    string          _data_path;
//    const char*     _data;
//    size_t          _data_size;
    string          _content_type;
    string          _connection;
    long long int   _content_length;

};


#endif //WEBSERV_HTTPRESPONSE_HPP
