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

#ifndef __HTTPRESPONSE_HPP__
#define __HTTPRESPONSE_HPP__
#pragma once

#include "HttpMessage.hpp"

class HttpResposnse : public HttpMessage {


public:

    HttpResposnse(unsigned code, const std::vector<std::string> &headerLines);
    ~HttpResposnse();

protected:

    HttpResposnse();

};


#endif //WEBSERV_HTTPRESPONSE_HPP
