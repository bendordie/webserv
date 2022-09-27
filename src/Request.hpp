/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cshells <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/31 16:06:15 by cshells           #+#    #+#             */
/*   Updated: 2022/01/31 16:06:16 by cshells          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __REQUEST_HPP__
#define __REQUEST_HPP__
#pragma once

#include <cstring>
#include <cstdlib>
#include <vector>
#include "HttpMessage.hpp"
#include "Utils.hpp"
#include "Location.hpp"
#include "WebServer.hpp"

class WebServer;

class Request : public HttpMessage {


public:

    Request(int responseCode, const string& protocol);
    Request(const string& method, const string& url, const string& protocol);
    ~Request();

    static int   idGenerator;

    static Request*               createRequest(const char* & bufferBegin, const char* & bufferEnd, int responseCode);
    static Request*               createRequest(const char* & bufferBegin, const char* & bufferEnd, const WebServer* server);
    static int                    countCRLFSequenceLenAtStart(const char* begin, const char* end);
    static bool                   isRequestLineValid(const char* requestBegin);
    static string                 defineRequestMethod(const char* begin);
    static const char*            defineHeaderEnd(const char* begin);
    static string                 defineRequestUrl(const char* begin);
    static pair<string, string>   splitHeaderOptionToKeyValue(const string& str);
    static void                   defineHeaderParams(bool keep_alive, long long int content_length, string& content_type);

    const int                     getId() const;
    const int                     getResponseCode() const;
    const string&                 getMethod() const;
    const Location*               getLocation() const;
    const char*                   getBuffedDataBegin() const;
    const char*                   getBuffedDataEnd() const;
    size_t                        getBuffedDataSize() const;
    size_t                        getHandledDataSize() const;
    const string&                 getDataPath() const;
    const string&                 getContentType() const;
    const long long               getContentLength() const;
    const size_t                  getHeaderSize() const;
    const size_t                  getSize() const;
    const size_t                  getRemainingDataSize() const;
    const string&                 getUrl() const;
    const int                     getRedirectCode() const;
    const string&                 getRedirectUrl() const;
    bool                          isRedirect() const;
    bool                          isChunked() const;
    bool                          isFullReceived() const;
    bool                          isKeepAlive() const;
    bool                          isFullProcessed() const;
    bool                          isHandledByCgi() const;
    bool                          isAcceptEncoding() const;

    void                          setResponseCode(int responseCode);
    void                          increaseHandledDataSize(size_t value);
    void                          setLocation(const Location* location);
    void                          setDataBegin(const char* data_begin);
    void                          setDataEnd(const char* data_end);
    void                          setBuffedDataSize(size_t size);
    void                          setHandledDataSize(size_t size);
    void                          setDataPath(const string& path);
    void                          setContentType(const string& contentType);
    void                          setContentLength(const long long int contentLength);
    void                          setUrl(const string& url);
    void                          setRedirectCode(const int code);
    void                          setRedirectUrl(const string& url);
    void                          setSize(size_t size);
    void                          setChunked(bool value);
    void                          setFullReceived(bool value);
    void                          setKeepAlive(bool value);
    void                          setFullProcessed(bool value);
    void                          setHandledByCgiFlag(bool value);
    void                          setAcceptEncodingFlag(bool value);

private:

    Request();

    bool                          initHeader(const char *headerBegin, const char *headerEnd);
    void                          initParams(const char *requestBegin, const char *headerEnd);
    void                          initData(const char *headerEnd, const char *bufferEnd);

    int               _id;
    int               _responseCode;
    string            _method;
    string            _url;

    const Location*   _location;

    int               _redirectCode;
    string            _redirectUrl;

    size_t            _size;
    size_t            _headerSize;
    long long         _contentLength;
    string            _contentType;

    const char*       _bufDataBegin;
    const char*       _bufDataEnd;
    size_t            _bufDataSize;
    size_t            _handledDataSize;
    string            _dataPath;

    bool              _chunked;
    bool              _acceptEncoding;
    bool              _fullReceived;
    bool              _fullProcessed;
    bool              _keepAlive;
    bool              _cgiHandled;

};

//cf49chh456

#endif //WEBSERV_HTTPREQUEST_HPP
