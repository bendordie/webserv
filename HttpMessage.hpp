/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpMessage.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cshells <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/31 16:06:03 by cshells           #+#    #+#             */
/*   Updated: 2022/01/31 16:06:04 by cshells          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __HTTPMESSAGE_HPP__
#define __HTTPMESSAGE_HPP__
#pragma once

#include <iostream>
#include <vector>

class HttpMessage {


public:

    HttpMessage(unsigned code, const std::vector<std::string> &headerLines);
    virtual ~HttpMessage();
    HttpMessage(const HttpMessage &other);
    HttpMessage& operator=(const HttpMessage &other);

    virtual std::vector<std::string>        getHeaderLines() const;
    virtual const unsigned&                 getCode() const;
    virtual const std::vector<uint8_t>&     getData() const = 0;


    void                                    setCode(unsigned code);
    void                                    setHeaderLines(const std::vector<std::string> &headerLines);
    void                                    setData(const char *begin, const char *end);


protected:

    HttpMessage();

    unsigned                    _code;
    std::vector<std::string>    _headerLines;
    std::vector<uint8_t>        _data;



};


#endif //WEBSERV_HTTPMESSAGE_HPP
