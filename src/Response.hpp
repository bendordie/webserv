/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cshells <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/31 19:40:16 by cshells           #+#    #+#             */
/*   Updated: 2022/01/31 19:40:18 by cshells          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef __RESPONSE_HPP__
#define __RESPONSE_HPP__

#include "WebServer.hpp"
//#include "WebSession.hpp"
#include "HttpMessage.hpp"
#include "Request.hpp"
#include "global_variables.hpp"

class Request;

class WebSession;

class WebServer;

class Response : public HttpMessage {

public:

    // SIMPLE / REDIRECT
    Response(const string &protocol, int statusCode, bool keepAliveFlag, bool acceptEncodingFlag,
             const PairList& cookies, const string &redirectUrl = "");
    // CHUNKED
    Response(const string &protocol, const string &dataPath, size_t dataBytesProcessed = 0);
    // FILE
    Response(const string &protocol, int statusCode, bool keepAliveFlag, bool acceptEncodingFlag,
             const string &dataPath, const map<string, string> &contentTypes, const PairList& cookies);
    // RAW DATA
    Response(const string &protocol, int statusCode, bool keepAliveFlag, bool acceptEncodingFlag,
             const char *data, size_t dataSize, string contentType, const PairList& cookies);
    ~Response();

    static Response*   createResponse(const WebSession* session, const Request *request, const WebServer *server,
                                      const char* data = nullptr);
    static string      makeAutoindexPage(const string &uri, const string &abs_path);
    static string      makeAutoindexLine(list<struct dirent>::iterator file, const string &path);

    string             makeStatusPage();
    bool               isFullProcessed() const;
    const string&      getHeader() const;
    size_t             getHandledDataSize() const;

protected:


private:

    void               makeResponseHeader(bool chunked, const string &fileTime, const PairList& cookies,
                                          const string &redirectUrl = "");
    static Response*   createGetResponse(const WebSession* session, const Request *request, const WebServer *server);
    static Response*   createPostResponse(const WebSession* session, const Request *request, const WebServer *server);
    static Response*   createDeleteResponse(const WebSession* session, const Request *request, const WebServer *server);

    int             _statusCode;
    string          _statusMsg;
    string          _header;
    int             _redirectCode;
    string          _redirectUrl;
    string          _dataPath;
    string          _contentType;
    long long int   _contentLength;
    size_t          _dataBytesHandled;
    size_t          _maxBufSize;
    bool            _keepAliveFlag;
    bool            _chunked;
    bool            _fullProcessed;

};


#endif //WEBSERV_HTTPRESPONSE_HPP
