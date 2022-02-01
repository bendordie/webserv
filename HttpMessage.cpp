/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpMessage.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cshells <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/31 16:06:08 by cshells           #+#    #+#             */
/*   Updated: 2022/01/31 16:06:09 by cshells          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpMessage.hpp"

HttpMessage::HttpMessage() {}

HttpMessage::HttpMessage(unsigned int code, const std::vector<std::string> &headerLines)
: _code(code), _headerLines(headerLines) {}

HttpMessage::~HttpMessage() {}

HttpMessage::HttpMessage(const HttpMessage &other) {}

HttpMessage &HttpMessage::operator=(const HttpMessage &other) {}

void HttpMessage::setCode(unsigned int code) { _code = code; }

void HttpMessage::setHeaderLines(const std::vector<std::string> &headerLines) { _headerLines = headerLines; }

void HttpMessage::setData(const char *begin, const char *end) {

    for (; begin != end; ++begin) {
        _data.assign(begin, end);
    }
}

const unsigned int &HttpMessage::getCode() const { return _code; }

std::vector<std::string> HttpMessage::getHeaderLines() const { return _headerLines; }

const std::vector<uint8_t> &HttpMessage::getData() const { return _data; }
