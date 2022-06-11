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
//#include "WebSession.hpp"
#include "HttpMessage.hpp"
#include "HttpRequest.hpp"

class HttpRequest;

class WebSession;

class WebServer;

struct Data {
    const char *data;
    size_t     size;
};


class HttpResponse : public HttpMessage {

private:

    enum class Status {
        CONTINUE = 100,
        MOVED_PERMANENTLY = 301,
        BAD_REQUEST = 400,
        FORBIDDEN = 403,
        NOT_FOUND = 404,
        NOT_ALLOWED = 405,
        INTERNAL_ERR = 500 };

public:

    HttpResponse(const string &protocol, const string &data_path, size_t data_bytes_processed = 0);
    HttpResponse(const string &protocol, Status status, const string &serverApplicationName);
    HttpResponse(const string &protocol, int status_code, const string &status_msg, const string &redirectValue);
    HttpResponse(const string &protocol, int status_code, const string &status_msg, const string &connection,
                 const string &data_path, const map<string, string> &content_types);
    HttpResponse(const string &protocol, int status_code, const string &status_msg, const string &connection,
                 const char *data, size_t data_size, string content_type);
    ~HttpResponse();

    static HttpResponse*   createResponse(HttpRequest *request, WebServer *server);
    static HttpResponse*   createGetResponse(HttpRequest *request, WebServer *server);
    static HttpResponse*   createPostResponse(HttpRequest *request, WebServer *server);
    static HttpResponse*   createDeleteResponse(HttpRequest *request, WebServer *server);
    static string          makeAutoindexPage(const string &uri, const string &abs_path);
    static string          makeAutoindexLine(list<struct dirent>::iterator file, const string &path);

    string                 makeStatusPage(int code, const string &serverApplicationName);
    bool                   isFullProcessed() const;
    const string&          getHeader() const;

//    static HttpResponse*       createNoBodyResponse(int status_code, string status_msg, string body_path = "");

protected:


private:

    void               makeResponseHeader(bool isHTTPv1_1, const string &file_time, const string &serverApplicationName);

    int                _statusCode;
    string             _statusMsg;
    string             _header;
    string             _redirect;
    string             _dataPath;
    string             _contentType;
    string             _connection;
    long long int      _contentLength;
    size_t             _dataBytesHandled;
    size_t             _dataBytesRemaining;
    size_t             _maxBufSize;
    bool               _chunked;
    bool               _fullProcessed;


};


#endif //WEBSERV_HTTPRESPONSE_HPP
