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

#include "HttpMessage.hpp"

class HttpRequest : public HttpMessage {


public:

    HttpRequest(unsigned code, const std::vector<std::string> &headerLines);
    ~HttpRequest();

    const std::string&              getRequestPath() const;
    const std::string&              getContentType() const;
    const long long int&            getContentLength() const;

    void                            setRequestPath(const std::string &path);
    void                            setContentType(const std::string &contentType);
    void                            setContentLength(const long long int &contentLength);

protected:

    HttpRequest();

private:

    std::string             _requestPath;
    std::string             _contentType;
    long long int           _contentLength;


};



#endif //WEBSERV_HTTPREQUEST_HPP
