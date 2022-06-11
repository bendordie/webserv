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
#include "ServerOptions.hpp"
#include "WebServer.hpp"

#define MAX_ENTITY_SIZE 4096 // TODO: replace

class WebServer;

class HttpRequest : public HttpMessage {


public:

    HttpRequest(const string &type, const string &uri, const string &protocol);
    ~HttpRequest();

    static int                  id_generator;

    static HttpRequest*         createRequest(const char* &bufferBegin, const char* &bufferEnd, const WebServer* server);



    static int                  countCRLFSequenceLenAtStart(const char *begin, const char *end);
    static bool                 isRequestLineValid(const char *request_begin);
    static string               defineRequestType(const char *begin);
    static const char*          defineHeaderEnd(const char *begin);
    static string               defineRequestUri(const char *begin);
    static string               defineRequestProtocol(const char *begin);
    static string               getOptionValueFromBuffer(const char *buffer, const char *optionName);
    static pair<string, string> splitHeaderOptionToKeyValue(const string &str);
    static void                 defineHeaderParams(bool keep_alive, long long int &content_length, string &content_type);

    int                         getId() const;
    const string&               getType() const;
    const char*                 getDataBegin() const;
    const char*                 getDataEnd() const;
    size_t                      getBuffedDataSize() const;
    size_t                      getHandledDataSize() const;
    const string&               getDataPath() const;
    const string&               getContentType() const;
    const long long             getContentLength() const;
    const size_t                getHeaderSize() const;
    const size_t                getSize() const;
    const size_t                getRemainingDataSize() const;
    const string&               getUri() const;
    const string&               getRedirect() const;
    bool                        isRedirect() const;
    bool                        isChunked() const;
    bool                        isFullReceived() const;
    bool                        isKeepAlive() const;
    bool                        isFullProcessed() const;

    void                        increaseHandledDataSize(size_t value);
    void                        setDataBegin(const char *data_begin);
    void                        setDataEnd(const char *data_end);
    void                        setBuffedDataSize(size_t size);
    void                        setHandledDataSize(size_t size);
    void                        setDataPath(const string &path);
    void                        setContentType(const string &contentType);
    void                        setContentLength(const long long int &contentLength);
    void                        setUri(const string &uri);
    void                        setRedirect(const string &redirect);
    void                        setSize(size_t size);
    void                        setChunked(bool value);
    void                        setFullReceived(bool value);
    void                        setKeepAlive(bool value);
    void                        setFullProcessed(bool value);

protected:

    HttpRequest();

private:

    bool                        initHeader(const char *headerBegin, const char *headerEnd);
    void                        initParams(const char *requestBegin, const char *headerEnd);
    void                        initData(const char *requestBegin, const char *headerEnd, const char *bufferEnd);

    int                     _id;
    string                  _type;
    string                  _uri;
    string                  _redirect;
    size_t                  _size;
    size_t                  _headerSize;
    long long               _contentLength;
    string                  _contentType;
    const char              *_bufDataBegin;
    const char              *_bufDataEnd;
    size_t                  _bufDataSize;
    size_t                  _handledDataSize;
    string                  _dataPath;
    bool                    _chunked;
    bool                    _fullReceived;
    bool                    _fullProcessed;
    bool                    _keepAlive;

};

//cf49chh456

#endif //WEBSERV_HTTPREQUEST_HPP
