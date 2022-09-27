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
#include "Request.hpp"
#include "Response.hpp"

using std::string;
using std::vector;
using std::mutex;
using std::list;
using std::queue;

class WebServer;

class Response;

class WebSession : public FdHandler {

    friend class WebServer;

    WebSession(WebServer* master, int fd);
    ~WebSession();

public:

    const TimePoint&   getStartTime() const;
    const PairList&    getCookiesList() const;

    static int   _idGenerator;

private:

    int               _id;
    char*             _mainBuffer;
    TimePoint         _beginTime;
    PairList          _cookies;
    bool              _isWritingPostDataOnDisk;
    bool              _keepAlive;
    bool              _processed;
    WebServer*        _master;
    vector<char>      _tempBuffer;
    queue<Request*>   _requestQueue;

    bool              receiveData(size_t& bytesRead);
    Response*         handleRequestByCgi(Request *request, const string& cgiPath);
    bool              writeDataOnDisk(Request* request, const Location* location);
    void              handleRequest();
    void              decodeChunkedTransfer(const char* begin, const char* end, Request* request);
    void              handleResponse();
    void              sendResponse(Response* response);
    bool              wantBeWritten() const override;
    void              handle(bool read, bool write) override;

};

#endif //WEBSERV_WEBSESSION_HPP
