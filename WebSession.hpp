/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebSession.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cshells <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/02 15:26:42 by cshells           #+#    #+#             */
/*   Updated: 2022/02/02 15:26:43 by cshells          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef __WEBSESSION_HPP__
#define __WEBSESSION_HPP__

#include <unistd.h>
#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <list>
#include "WebServer.hpp"
#include "FdHandler.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

using namespace std;

enum {
    buffer_size = 4096,
    backlog = 16
};

class WebServer;

class CgiHandler {

public:

    void    handleRequest(HttpRequest *request);
};

class HttpResponse;

class WebSession : public FdHandler {

    friend class WebServer;

    WebSession(WebServer *master, int fd);
    ~WebSession();


    bool                _keep_alive;
    bool                _processed;
    bool                _request_received;
    WebServer           *_master;
    HttpRequest*        _request;
    vector<char>        _tmp_data;
    list<HttpRequest*>  _request_list;
    CgiHandler          *_cgi;

    bool                receiveData(char *buffer, size_t &bytesRead);
    bool                handlePostData(HttpRequest *request);
    int                 countControlCharSequenceLen(const char *start, const char *end);
    string              defineRequestType(const char *begin);
    const char*         defineHeaderEnd(const char *begin);
    void                handleRequest();
    void                handleResponse();
    void                sendResponse(HttpResponse *response);
    virtual bool        wantWrite();
    virtual void        Handle(bool read, bool write);

//    void            sendMessage(const char *msg);
//    void            ReadAndIgnore();
//    void            ReadAndCheck();
//    void            CheckLines();
//    void            ProcessLine(const char *str);
    // TODO: duration


    //TODO: request list

//    int         _buf_used;
//    char        *_name;
//    bool        _ignoring;


//    bool            handlePostHeader(Client *client);
//    int             countEmptyHeaderLines(const char *buffer, HttpRequest *request);
//    void            defineHeaderEnd(char *buffer, const size_t &bytesRead, char **headerEnd, size_t &headerSize);
//    std::string     extractRequestPath(const char *buffer);



};

#endif //WEBSERV_WEBSESSION_HPP
