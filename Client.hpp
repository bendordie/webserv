/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cshells <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/16 17:57:35 by cshells           #+#    #+#             */
/*   Updated: 2021/12/16 17:57:36 by cshells          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef __CLIENT_HPP__
#define __CLIENT_HPP__

#include <iostream>
#include <vector>
#include <chrono>
#include <sys/time.h>
#include "HttpMessage.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

class Client {

public:

    Client();
    Client(int socket);
    ~Client();
    Client(const Client &other);
    Client& operator=(const Client &other);

    const int&                      getId() const;
    const int&                      getSocket() const;
    const std::vector<uint8_t>&     getData() const;
    const std::vector<uint8_t>&     getResponse() const;
    const struct timeval&           getLastActionTime() const;
    const int&                      getRequestType() const;
    const std::string&              getRequestHeader() const;
    const std::string&              getRequestPath() const;
    const long long int&            getPostContentLen() const;
    const std::string&              getPostContentType() const;

    bool                            isKeepAlive() const;
    bool                            isProcessed() const;
    bool                            isTimeout() const;

    void                            setResponse(const char *data, size_t size);
    void                            setKeepAlive(bool value);
    void                            setProcessed(bool value);
    void                            setTimeout(bool value);
    void                            setRequestType(int value);
    void                            setRequestHeader(const char *begin, const char* end);
    void                            setRequestPath(const std::string &path);
    void                            setPostContentLen(const long long int &length);
    void                            setPostContentType(const std::string &type);
    void                            setLastActionTime(const struct timeval &tv);

    void                            writeData(const char *begin, const char *end);
    void                            clearData();

    static int                      count;

private:



    int                             _id;
    int                             _socket;

    int                             _request_type;
    std::string                     _request_header;
    std::string                     _request_path;
    std::vector<uint8_t>            _data_recieved;
    long long int                   _post_content_len;
    std::string                     _post_content_type;

    std::vector<uint8_t>            _response;

    bool                            _ready_to_send;
    bool                            _keep_alive;
    bool                            _processed;
    bool                            _timeout;
    struct timeval                  _time_last_action;

};


#endif //CLIENT_HPP
