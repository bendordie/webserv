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
#include <chrono>
#include <queue>
#include "WebServer.hpp"
#include "FdHandler.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

using std::string;
using std::vector;
using std::mutex;
using std::list;
using std::queue;

enum {
    buffer_size = 4096,
    backlog = 16
};

//const int  MAX_ENTITY_SIZE = 4000;

class WebServer;

class ServerOptions;

class CgiHandler {

public:

    void    handleRequest(HttpRequest *request);
};

class HttpResponse;

class WebSession : public FdHandler {

    using TimePoint = chrono::time_point<chrono::steady_clock>;
    using SessionDuration = chrono::duration<chrono::seconds>;

    friend class WebServer;

    WebSession(WebServer *master, int fd);
    ~WebSession();

public:

    const TimePoint&    getStartTime() const;

    static int              _idGenerator;

private:

    int                     _id;
    TimePoint               _beginTime;
    bool                    _keepAlive;
    bool                    _processed;
    WebServer               *_master;
    string                  _tempRequestType;
    vector<char>            _tempBuffer;
    queue<HttpRequest*>     _requestQueue;

    bool                receiveData(char *buffer, size_t &bytesRead);
    bool                handlePostData(HttpRequest *request, const ServerOptions *url_traits);
//    int                 countControlCharSequenceLen(const char *begin, const char *end);
//    bool                isRequestLineValid(const char *request_begin) const;
//    string              defineRequestType(const char *begin);
    void                handleRequest();
    void                handlePostRequest(HttpRequest *request);
    void                decodeChunkedTransfer(HttpRequest *request);
    HttpRequest*        addNewRequest(const string &type, const char *header_begin,
                                      const char *header_end, size_t buf_size);
//    bool                checkThis(const char *header_end, size_t request_size1, size_t request_size2);
    void                handleResponse();
    void                sendResponse(HttpResponse *response);
    bool                wantBeWritten() const;
    void                handle(bool read, bool write);


};

#endif //WEBSERV_WEBSESSION_HPP
