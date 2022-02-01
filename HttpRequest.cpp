/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cshells <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/31 16:06:18 by cshells           #+#    #+#             */
/*   Updated: 2022/01/31 16:06:19 by cshells          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"

HttpRequest::HttpRequest() {}

HttpRequest::HttpRequest(unsigned int code, const std::vector<std::string> &headerLines)
: HttpMessage(code, headerLines) {}

HttpRequest::~HttpRequest() {}

void HttpRequest::setRequestPath(const std::string &path) { _requestPath = path; }


void HttpRequest::setContentType(const std::string &contentType) { _contentType = contentType; }

void HttpRequest::setContentLength(const long long &contentLength) { _contentLength = contentLength; }

const std::string &HttpRequest::getRequestPath() const { return _requestPath; }


const std::string &HttpRequest::getContentType() const { return _contentType; }

const long long &HttpRequest::getContentLength() const { return _contentLength; }